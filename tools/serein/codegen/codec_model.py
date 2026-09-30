"""Turn proto files with file options into C++ types with JSON codecs."""

from dataclasses import dataclass, field

from model import RULES_EXTENSION, SchemaError, camel_upper, cpp_string

FILE_EXTENSION = "[serein.options.v1.file]"
DOCUMENT_EXTENSION = "[serein.options.v1.document]"
CODEC_EXTENSION = "[serein.options.v1.codec]"
SCALARS = {
    "TYPE_BOOL": ("bool", "false"),
    "TYPE_INT32": ("int", "0"),
    "TYPE_INT64": ("qint64", "0"),
    "TYPE_STRING": ("QString", ""),
    "TYPE_BYTES": ("QByteArray", ""),
}
RULE_FAMILY = {
    "TYPE_INT32": "int32", "TYPE_INT64": "int64", "TYPE_STRING": "string",
    "TYPE_ENUM": "enum",
}
COMPARE = {"const": "==", "gte": ">=", "gt": ">", "lte": "<=", "lt": "<"}


@dataclass
class EnumValue:
    cpp: str
    json: str
    number: int


@dataclass
class Enum:
    name: str
    values: list


@dataclass
class Field:
    member: str
    key: str
    cpp_type: str
    default: str
    checks: list = field(default_factory=list)


@dataclass
class Message:
    name: str
    fields: list
    checks: list
    version: int = 0
    validator: str = ""
    require_fields: bool = False


@dataclass
class CodecFile:
    source: str
    namespace: str
    header: str
    implementation: str
    enums: list
    messages: list


def quoted_key(key):
    return f'QLatin1StringView("{key}")'


def enum_value_name(enum_name, value_name):
    prefix = "".join(
        "_" + ch if ch.isupper() and i else ch.upper()
        for i, ch in enumerate(enum_name)).upper() + "_"
    if not value_name.startswith(prefix):
        raise SchemaError(f"enum value {value_name} must start with {prefix}")
    return camel_upper(value_name[len(prefix):].lower())


def build_enum(enum):
    values = [EnumValue(enum_value_name(enum["name"], value["name"]), value["name"],
                        int(value.get("number", 0))) for value in enum.get("value", [])]
    if not values or values[0].number != 0:
        raise SchemaError(f"enum {enum['name']} must start with a zero value")
    return Enum(enum["name"], values)


def scalar_condition(kind, name, value, expression, enum=None):
    if name in COMPARE and kind in ("int32", "int64"):
        return f"{expression} {COMPARE[name]} {int(value)}"
    if name in ("in", "notIn") and kind in ("int32", "int64", "enum"):
        subject = f"int({expression})" if kind == "enum" else expression
        if name == "notIn":
            return " && ".join(f"{subject} != {int(item)}" for item in value)
        return "(" + " || ".join(f"{subject} == {int(item)}" for item in value) + ")"
    if kind == "enum" and name == "definedOnly":
        return None
    if kind == "string":
        if name in ("minLen", "maxLen"):
            sign = ">=" if name == "minLen" else "<="
            return f"{expression}.toUcs4().size() {sign} {int(value)}"
        if name == "pattern":
            return f"Codec::Matches({expression}, QString::fromUtf8({cpp_string(value)}))"
        if name == "uuid" and value:
            return f"Codec::IsUuid({expression})"
        if name == "const":
            return f"{expression} == QString::fromUtf8({cpp_string(value)})"
        if name == "in":
            return "(" + " || ".join(
                f"{expression} == QString::fromUtf8({cpp_string(item)})"
                for item in value) + ")"
        if name == "notIn":
            return " && ".join(
                f"{expression} != QString::fromUtf8({cpp_string(item)})"
                for item in value)
    raise SchemaError(f"unsupported {kind} rule '{name}'")


def scalar_conditions(kind, rules, expression, where):
    conditions = []
    for name, value in rules.get(kind, {}).items():
        try:
            condition = scalar_condition(kind, name, value, expression)
        except SchemaError as error:
            raise SchemaError(f"{where}: {error}") from None
        if condition:
            conditions.append(condition)
    unknown = set(rules) - {kind, "repeated"}
    if unknown:
        raise SchemaError(f"{where}: unsupported validation {sorted(unknown)}")
    return conditions


def fail_line(path, indent):
    return (f"{indent}\treturn Codec::Fail(error, {path}, "
            'QString::fromLatin1("violates the schema rules"));')


def field_checks(item, member, key, kind, repeated, optional, message_type, where):
    rules = item.get("options", {}).get(RULES_EXTENSION, {})
    path = f"Codec::Child(path, {quoted_key(key)})"
    lines = []
    if repeated:
        repeated_rules = rules.get("repeated", {})
        for name, value in repeated_rules.items():
            if name in ("minItems", "maxItems"):
                sign = ">=" if name == "minItems" else "<="
                condition = f"qsizetype(value.{member}.size()) {sign} {int(value)}"
            elif name == "unique" and value:
                condition = f"Codec::Unique(value.{member})"
            elif name == "items":
                continue
            else:
                raise SchemaError(f"{where}: unsupported repeated rule '{name}'")
            lines += [f"\tif (!({condition})) {{", fail_line(path, "\t"), "\t}"]
        items = repeated_rules.get("items", {})
        conditions = (scalar_conditions(kind, items, "item", where)
                      if kind and items else [])
        if conditions or message_type:
            lines.append(f"\tfor (auto i = qsizetype(); i != qsizetype(value.{member}.size()); ++i) {{")
            lines.append(f"\t\tconst auto &item = value.{member}[i];")
            item_path = f"Codec::Item({path}, i)"
            if conditions:
                lines += [f"\t\tif (!({' && '.join(conditions)})) {{", fail_line(item_path, "\t\t"), "\t\t}"]
            if message_type:
                lines += [f"\t\tif (!Validate(item, error, {item_path})) {{",
                          "\t\t\treturn false;", "\t\t}"]
            lines.append("\t}")
        return lines
    subject = f"(*value.{member})" if optional else f"value.{member}"
    conditions = scalar_conditions(kind, rules, subject, where) if kind else []
    if conditions:
        condition = " && ".join(conditions)
        if optional:
            condition = f"!value.{member} || ({condition})"
        lines += [f"\tif (!({condition})) {{", fail_line(path, "\t"), "\t}"]
    if message_type:
        guard = f"value.{member} && " if optional else ""
        lines += [f"\tif ({guard}!Validate({subject}, error, {path})) {{",
                  "\t\treturn false;", "\t}"]
    return lines


def map_checks(item, member, value_kind, value_message, where):
    rules = item.get("options", {}).get(RULES_EXTENSION, {})
    unknown = set(rules) - {"map"}
    if unknown:
        raise SchemaError(f"{where}: unsupported map rules {sorted(unknown)}")
    map_rules = rules.get("map", {})
    path = f"Codec::Child(path, {quoted_key(item['jsonName'])})"
    lines = []
    for name, value in map_rules.items():
        if name in ("minPairs", "maxPairs"):
            sign = ">=" if name == "minPairs" else "<="
            condition = f"qsizetype(value.{member}.size()) {sign} {int(value)}"
            lines += [f"\tif (!({condition})) {{", fail_line(path, "\t"), "\t}"]
        elif name not in ("keys", "values"):
            raise SchemaError(f"{where}: unsupported map rule '{name}'")
    keys = map_rules.get("keys", {})
    values = map_rules.get("values", {})
    key_conditions = scalar_conditions("string", keys, "key", where) if keys else []
    value_conditions = (scalar_conditions(value_kind, values, "item", where)
                        if value_kind and values else [])
    if key_conditions or value_conditions or value_message:
        lines.append(f"\tfor (const auto &[key, item] : value.{member}) {{")
        entry_path = f"Codec::Entry({path}, key)"
        for conditions in (key_conditions, value_conditions):
            if conditions:
                lines += [f"\t\tif (!({' && '.join(conditions)})) {{",
                          fail_line(entry_path, "\t\t"), "\t\t}"]
        if value_message:
            lines += [f"\t\tif (!Validate(item, error, {entry_path})) {{",
                      "\t\t\treturn false;", "\t\t}"]
        lines.append("\t}")
    return lines


def value_type(item, local_types, where):
    if item["type"] in SCALARS:
        return SCALARS[item["type"]][0], RULE_FAMILY.get(item["type"]), None
    if item["type"] in ("TYPE_ENUM", "TYPE_MESSAGE"):
        if item["typeName"] not in local_types:
            raise SchemaError(f"{where}: {item['typeName']} must be declared in the same file")
        name = item["typeName"].rsplit(".", 1)[-1]
        message = name if item["type"] == "TYPE_MESSAGE" else None
        return name, RULE_FAMILY.get(item["type"]), message
    raise SchemaError(f"{where}: unsupported type {item['type']}")


def build_map_field(item, entry, local_types, where):
    key, value = sorted(entry["field"], key=lambda field: field["number"])
    if key["type"] != "TYPE_STRING":
        raise SchemaError(f"{where}: map keys must be strings")
    cpp, kind, message = value_type(value, local_types, where)
    member = item["jsonName"]
    checks = map_checks(item, member, kind, message, where)
    return Field(member, item["jsonName"], f"std::map<QString, {cpp}>", "", checks), message


def build_field(item, local_types, where, map_entries=None):
    if item.get("type") == "TYPE_GROUP" or "oneofIndex" in item and not item.get("proto3Optional"):
        raise SchemaError(f"{where}: oneofs are not supported")
    if map_entries and item.get("typeName") in map_entries:
        return build_map_field(item, map_entries[item["typeName"]], local_types, where)
    repeated = item.get("label") == "LABEL_REPEATED"
    optional = bool(item.get("proto3Optional"))
    kind = RULE_FAMILY.get(item["type"])
    message_type = None
    if item["type"] in SCALARS:
        cpp, default = SCALARS[item["type"]]
    elif item["type"] in ("TYPE_ENUM", "TYPE_MESSAGE"):
        name = item["typeName"].rsplit(".", 1)[-1]
        if item["typeName"] not in local_types:
            raise SchemaError(f"{where}: {item['typeName']} must be declared in the same file")
        cpp = name
        default = f"{name}::{local_types[item['typeName']]}" if item["type"] == "TYPE_ENUM" else ""
        message_type = name if item["type"] == "TYPE_MESSAGE" else None
    else:
        raise SchemaError(f"{where}: unsupported type {item['type']}")
    if repeated:
        cpp, default = f"std::vector<{cpp}>", ""
    elif optional:
        cpp, default = f"std::optional<{cpp}>", ""
    member = item["jsonName"]
    checks = field_checks(item, member, item["jsonName"], kind, repeated, optional,
                          message_type, where)
    return Field(member, item["jsonName"], cpp, default, checks), message_type


def order_messages(messages, dependencies):
    ordered, visiting, done = [], set(), set()

    def visit(name):
        if name in done:
            return
        if name in visiting:
            raise SchemaError(f"message {name} is part of a reference cycle")
        visiting.add(name)
        for dependency in dependencies[name]:
            visit(dependency)
        visiting.discard(name)
        done.add(name)
        ordered.append(messages[name])

    for name in messages:
        visit(name)
    return ordered


def build_file(file):
    source = file["name"]
    options = file.get("options", {}).get(FILE_EXTENSION)
    if not options or not options.get("cppNamespace"):
        return None
    package = "." + file.get("package", "")
    local_types = {}
    enums = [build_enum(enum) for enum in file.get("enumType", [])]
    for enum in enums:
        local_types[f"{package}.{enum.name}"] = enum.values[0].cpp
    map_entries = {}
    for message in file.get("messageType", []):
        nested = message.get("nestedType", [])
        if message.get("enumType") or any(
                not entry.get("options", {}).get("mapEntry") for entry in nested):
            raise SchemaError(f"{source}:{message['name']}: nested types are not supported")
        for entry in nested:
            map_entries[f"{package}.{message['name']}.{entry['name']}"] = entry
        local_types[f"{package}.{message['name']}"] = None
    messages, dependencies = {}, {}
    for message in file.get("messageType", []):
        where = f"{source}:{message['name']}"
        fields, needs = [], set()
        for item in message.get("field", []):
            built, dependency = build_field(
                item, local_types, f"{where}.{item['name']}", map_entries)
            if built.key == "version" and DOCUMENT_EXTENSION in message.get("options", {}):
                raise SchemaError(f"{where}: 'version' is reserved in documents")
            fields.append(built)
            if dependency:
                needs.add(dependency)
        document = message.get("options", {}).get(DOCUMENT_EXTENSION, {})
        messages[message["name"]] = Message(
            name=message["name"], fields=fields,
            checks=[line for built in fields for line in built.checks],
            version=int(document.get("version", 0)),
            validator=document.get("validator", ""),
            require_fields=bool(message.get("options", {}).get(
                CODEC_EXTENSION, {}).get("requireFields", False)))
        dependencies[message["name"]] = needs
    directory = source.split("/")[1]
    stem = source.rsplit("/", 1)[-1].removesuffix(".proto")
    return CodecFile(
        source=f"proto/{source}",
        namespace=options["cppNamespace"],
        header=f"{directory}/{stem}.h",
        implementation=f"{directory}/{stem}.cpp",
        enums=enums,
        messages=order_messages(messages, dependencies),
    )


def build_files(image):
    files = []
    for file in image.get("file", []):
        if file["name"].startswith("serein/"):
            built = build_file(file)
            if built:
                files.append(built)
    return files
