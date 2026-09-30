"""Turn a Buf JSON image into settings pages for the C++ templates."""

import json
from dataclasses import dataclass, field

PAGE_EXTENSION = "[serein.options.v1.page]"
FIELD_EXTENSION = "[serein.options.v1.field]"
RULES_EXTENSION = "[buf.validate.field]"
SETTINGS_PREFIX = "serein/settings/"

SCOPES = {"SCOPE_DEVICE": "Device", "SCOPE_ACCOUNT": "Account"}
CATEGORIES = {
    "CATEGORY_INTERFACE": "Interface",
    "CATEGORY_CHATS": "Chats",
    "CATEGORY_MESSAGES": "Messages",
    "CATEGORY_COMPOSE": "Compose",
    "CATEGORY_MENU": "Menu",
    "CATEGORY_MEDIA": "Media",
    "CATEGORY_PRIVACY": "Privacy",
    "CATEGORY_SERVICES": "Services",
    "CATEGORY_RULES": "Rules",
}
REFRESH_FLAGS = {
    "REFRESH_MESSAGE_VIEW": "RefreshMessageView",
    "REFRESH_DIALOG_LIST": "RefreshDialogList",
    "REFRESH_COMPOSE_BUTTONS": "RefreshComposeButtons",
}
TYPES = {
    "TYPE_BOOL": ("bool", "bool"),
    "TYPE_INT32": ("int", "int32"),
    "TYPE_STRING": ("QString", "string"),
    "TYPE_BYTES": ("QByteArray", "bytes"),
}
INT_RULES = {"gte": ">=", "gt": ">", "lte": "<=", "lt": "<", "const": "=="}
STRING_RULES = {"min_len", "max_len", "pattern", "const"}


class SchemaError(Exception):
    pass


@dataclass
class Option:
    cpp_name: str
    ctype: str
    key: str
    scope: str
    fallback: str
    category: str
    title: str
    flags: list
    validator: list = field(default_factory=list)

    @property
    def constexpr(self):
        return self.ctype in ("bool", "int")

    @property
    def flags_expression(self):
        if not self.flags:
            return "0"
        return " | ".join(f"static_cast<unsigned>(Flag::{flag})" for flag in self.flags)

    @property
    def initializer(self):
        args = [
            f'"{self.key}"', f"Scope::{self.scope}", self.fallback,
            f"Category::{self.category}", f'"{self.title}"', self.flags_expression,
        ]
        lines = [f"\t{arg}," for arg in args]
        if not self.validator:
            lines[-1] = lines[-1][:-1] + " };"
        elif len(self.validator) == 1:
            lines.append(f"\t{self.validator[0]} }};")
        else:
            lines += [f"\t{line}" for line in self.validator]
            lines.append("\t} };")
        return lines


@dataclass
class Page:
    source: str
    namespace: str
    header: str
    options: list
    custom_validators: list
    needs_regex: bool


def camel_upper(name):
    return "".join(part[:1].upper() + part[1:] for part in name.split("_"))


def cpp_string(value):
    return json.dumps(value, ensure_ascii=False)


def fallback_literal(ctype, options):
    if ctype == "bool":
        return "true" if options.get("defaultBool", False) else "false"
    if ctype == "int":
        return str(int(options.get("defaultInt", 0)))
    value = options.get("defaultString", "")
    if not value:
        return f"{ctype}()"
    if ctype == "QString":
        return f"QString::fromUtf8({cpp_string(value)})"
    return f"QByteArray({cpp_string(value)})"


RULE_ORDER = ["const", "gte", "gt", "lte", "lt", "in", "notIn"]


def int_conditions(rules, where):
    conditions = []
    ordered = sorted(rules.items(), key=lambda item: (
        RULE_ORDER.index(item[0]) if item[0] in RULE_ORDER else len(RULE_ORDER)))
    for name, value in ordered:
        if name in INT_RULES:
            conditions.append(f"value {INT_RULES[name]} {int(value)}")
        elif name in ("in", "notIn"):
            joined = " || ".join(f"value == {int(item)}" for item in value)
            conditions.append(f"({joined})" if name == "in" else f"!({joined})")
        else:
            raise SchemaError(f"{where}: unsupported int32 rule '{name}'")
    return conditions


def string_conditions(rules, where):
    conditions = []
    for name, value in rules.items():
        snake = "".join("_" + ch.lower() if ch.isupper() else ch for ch in name)
        if snake not in STRING_RULES:
            raise SchemaError(f"{where}: unsupported string rule '{name}'")
        if snake == "min_len":
            conditions.append(f"value.toUcs4().size() >= {int(value)}")
        elif snake == "max_len":
            conditions.append(f"value.toUcs4().size() <= {int(value)}")
        elif snake == "const":
            conditions.append(f"value == QString::fromUtf8({cpp_string(value)})")
        else:
            conditions.append(
                f"QRegularExpression(QString::fromUtf8({cpp_string(value)}))"
                ".match(value).hasMatch()")
    return conditions


def validator_lines(ctype, fallback, rules, where):
    if not rules:
        return []
    unknown = set(rules) - {"int32", "string"}
    if unknown:
        raise SchemaError(f"{where}: unsupported validation {sorted(unknown)}")
    if "int32" in rules and ctype == "int":
        conditions = int_conditions(rules["int32"], where)
    elif "string" in rules and ctype == "QString":
        conditions = string_conditions(rules["string"], where)
    else:
        raise SchemaError(f"{where}: validation does not match the field type")
    joined = " && ".join(f"({condition})" for condition in conditions)
    return [
        f"[](const {ctype} &value) {{",
        f"\treturn (value == {fallback})",
        f"\t\t|| ({joined});",
    ]


def build_option(message_field, page, where):
    if message_field.get("label") == "LABEL_REPEATED":
        raise SchemaError(f"{where}: repeated settings are not supported")
    if message_field.get("type") not in TYPES:
        raise SchemaError(f"{where}: unsupported type {message_field.get('type')}")
    ctype = TYPES[message_field["type"]][0]
    options = message_field.get("options", {})
    custom = options.get(FIELD_EXTENSION, {})
    name = message_field["name"]
    fallback = fallback_literal(ctype, custom)
    flags = [REFRESH_FLAGS[item] for item in custom.get("refresh", [])]
    if custom.get("restart"):
        flags.append("RequiresRestart")
    if custom.get("hidden"):
        flags.append("Hidden")
    lines = validator_lines(ctype, fallback, options.get(RULES_EXTENSION), where)
    if custom.get("validator"):
        if lines:
            raise SchemaError(f"{where}: both a C++ validator and rules are set")
        lines = [f"&{custom['validator']}"]
    return Option(
        cpp_name=custom.get("cppName") or "k" + camel_upper(name),
        ctype=ctype,
        key="serein." + message_field["jsonName"],
        scope=SCOPES[custom.get("scope", page["scope"])],
        fallback=fallback,
        category=CATEGORIES[page["category"]],
        title=custom.get("title") or "lng_serein_" + name,
        flags=flags,
        validator=lines,
    )


def build_page(source, message):
    page = message.get("options", {}).get(PAGE_EXTENSION)
    if not page:
        return None
    where = f"{source}:{message['name']}"
    for required in ("cppNamespace", "scope", "category"):
        if not page.get(required):
            raise SchemaError(f"{where}: page option '{required}' is required")
    options = [build_option(item, page, f"{where}.{item['name']}")
               for item in message.get("field", [])]
    names = [option.cpp_name for option in options]
    if len(set(names)) != len(names):
        raise SchemaError(f"{where}: duplicate C++ names")
    custom = []
    for item, option in zip(message.get("field", []), options):
        validator = item.get("options", {}).get(FIELD_EXTENSION, {}).get("validator")
        if validator and (validator, option.ctype) not in custom:
            custom.append((validator, option.ctype))
    stem = source.rsplit("/", 1)[-1].removesuffix(".proto")
    return Page(
        source=f"proto/{source}",
        namespace=page["cppNamespace"],
        header=f"settings/{stem}.h",
        options=options,
        custom_validators=custom,
        needs_regex=any("QRegularExpression" in line
                        for option in options for line in option.validator),
    )


def build_pages(image):
    pages = []
    for file in image.get("file", []):
        if not file["name"].startswith(SETTINGS_PREFIX):
            continue
        for message in file.get("messageType", []):
            page = build_page(file["name"], message)
            if page:
                pages.append(page)
    keys = [option.key for page in pages for option in page.options]
    duplicates = sorted({key for key in keys if keys.count(key) > 1})
    if duplicates:
        raise SchemaError(f"duplicate storage keys: {duplicates}")
    return pages
