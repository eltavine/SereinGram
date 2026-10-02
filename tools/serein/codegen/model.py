"""Turn a Buf JSON image into settings pages for the C++ templates."""

import json
import re
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
    name: str = ""
    keywords: list = field(default_factory=list)
    custom_ui: bool = False

    @property
    def toggle(self):
        return self.ctype == "bool" and not self.custom_ui and "Hidden" not in self.flags

    @property
    def accessor(self):
        name = self.cpp_name
        return name[1:] if name[:1] == "k" and name[1:2].isupper() else name

    @property
    def account(self):
        return self.scope == "Account"

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
            f'"{self.key}"',
            f"Scope::{self.scope}",
            self.fallback,
            f"Category::{self.category}",
            f'"{self.title}"',
            self.flags_expression,
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
class Row:
    cpp_name: str
    title: str
    id: str
    keywords: str
    disabled_by: str = ""


@dataclass
class LayoutItem:
    kind: str
    index: int = 0
    member: str = ""
    id: str = ""
    title: str = ""
    keywords: str = ""
    cpp_name: str = ""
    minimum: int = 0
    maximum: int = 0
    zero_label: str = ""
    count_format: str = ""
    values: list = field(default_factory=list)
    labels: list = field(default_factory=list)
    suffix: str = ""
    hint: str = ""
    placeholder: str = ""
    hidden_by: str = ""


@dataclass
class Page:
    source: str
    namespace: str
    header: str
    options: list
    custom_validators: list
    needs_codec: bool
    rows_header: str = ""
    rows: list = field(default_factory=list)
    layout: list = field(default_factory=list)
    customs: list = field(default_factory=list)
    stem: str = ""
    subpage: dict = field(default_factory=dict)

    @property
    def hook_types(self):
        return {option.ctype for option in self.options}

    @property
    def has_account(self):
        return any(option.account for option in self.options)


def camel_upper(name):
    return "".join(part[:1].upper() + part[1:] for part in name.split("_"))


def camel_lower(name):
    upper = camel_upper(name)
    return upper[:1].lower() + upper[1:]


def cpp_keywords(words):
    return ", ".join(f"u{cpp_string(word)}_q" for word in words)


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
    ordered = sorted(
        rules.items(),
        key=lambda item: RULE_ORDER.index(item[0]) if item[0] in RULE_ORDER else len(RULE_ORDER),
    )
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
            conditions.append(f"Codec::Matches(value, QString::fromUtf8({cpp_string(value)}))")
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
        name=name,
        keywords=list(custom.get("keywords", [])),
        custom_ui=bool(custom.get("customUi")),
    )


def build_page(source, message):
    page = message.get("options", {}).get(PAGE_EXTENSION)
    if not page:
        return None
    where = f"{source}:{message['name']}"
    for required in ("cppNamespace", "scope", "category"):
        if not page.get(required):
            raise SchemaError(f"{where}: page option '{required}' is required")
    options = [
        build_option(item, page, f"{where}.{item['name']}") for item in message.get("field", [])
    ]
    names = [option.cpp_name for option in options]
    if len(set(names)) != len(names):
        raise SchemaError(f"{where}: duplicate C++ names")
    custom = []
    for item, option in zip(message.get("field", []), options, strict=True):
        validator = item.get("options", {}).get(FIELD_EXTENSION, {}).get("validator")
        if validator and (validator, option.ctype) not in custom:
            custom.append((validator, option.ctype))
    stem = source.rsplit("/", 1)[-1].removesuffix(".proto")
    rows, layout, customs = build_layout(message, options, stem, where)
    subpage = build_subpage(page.get("subpage"), where)
    return Page(
        source=f"proto/{source}",
        namespace=page["cppNamespace"],
        header=f"settings/{stem}.h",
        options=options,
        custom_validators=custom,
        needs_codec=any("Codec::" in line for option in options for line in option.validator),
        rows_header=f"{stem}_rows.h" if (layout or subpage) else "",
        stem=stem,
        rows=rows,
        layout=layout,
        customs=customs,
        subpage=subpage,
    )


def build_subpage(subpage, where):
    if not subpage:
        return {}
    for required in ("title", "icon"):
        if not subpage.get(required):
            raise SchemaError(f"{where}: subpage '{required}' is required")
    if not re.fullmatch(r"[A-Za-z_][A-Za-z0-9_]*", subpage["icon"]):
        raise SchemaError(f"{where}: subpage icon must be a style name")
    return {
        "title": subpage["title"],
        "icon": subpage["icon"],
        "keywords": cpp_keywords(subpage.get("keywords", [])),
    }


def build_layout(message, options, stem, where):
    by_name = {option.name: option for option in options}
    rows, layout, customs = [], [], []
    for item, option in zip(message.get("field", []), options, strict=True):
        if "Hidden" in option.flags:
            continue
        custom = item.get("options", {}).get(FIELD_EXTENSION, {})
        place = f"{where}.{item['name']}"
        section = custom.get("section")
        if section:
            for required in ("title", "id"):
                if not section.get(required):
                    raise SchemaError(f"{place}: section '{required}' is required")
            layout.append(
                LayoutItem(
                    "section",
                    id=f"serein/{stem}/{section['id']}",
                    title=section["title"],
                    keywords=cpp_keywords(section.get("keywords", [])),
                )
            )
        if "number" in custom and not option.custom_ui:
            layout.append(number_item(item, option, custom["number"], stem, place))
        elif "choice" in custom and not option.custom_ui:
            layout.append(choice_item(item, option, custom["choice"], stem, place))
        elif "text" in custom and not option.custom_ui:
            layout.append(
                text_item(
                    option,
                    custom["text"],
                    stem,
                    place,
                    visible_toggle(by_name, custom.get("disabledBy", ""), option, place),
                )
            )
        elif option.toggle:
            disabled_by = visible_toggle(by_name, custom.get("disabledBy", ""), option, place)
            layout.append(LayoutItem("toggle", index=len(rows)))
            rows.append(
                Row(
                    cpp_name=option.cpp_name,
                    title=option.title,
                    id=f"serein/{stem}/{option.name.replace('_', '-')}",
                    keywords=cpp_keywords(option.keywords),
                    disabled_by=disabled_by,
                )
            )
        else:
            member = camel_lower(option.name)
            customs.append(member)
            layout.append(LayoutItem("custom", member=member))
        if custom.get("note"):
            layout.append(LayoutItem("note", title=custom["note"]))
    return rows, layout, customs


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
    ids = [row.id for page in pages for row in page.rows]
    ids += [
        item.id
        for page in pages
        for item in page.layout
        if item.kind in ("section", "number", "choice", "text")
    ]
    duplicates = sorted({id for id in ids if ids.count(id) > 1})
    if duplicates:
        raise SchemaError(f"duplicate settings row ids: {duplicates}")
    return pages


def visible_toggle(by_name, name, option, place):
    if not name:
        return ""
    source = by_name.get(name)
    if not source or not source.toggle or source is option:
        raise SchemaError(
            f"{place}: disabled_by '{name}' is not a visible boolean option of this page"
        )
    return source.cpp_name


def text_item(option, text, stem, place, hidden_by):
    if option.ctype != "QString":
        raise SchemaError(f"{place}: text inputs need a string option")
    if not text.get("placeholder"):
        raise SchemaError(f"{place}: text inputs need a placeholder")
    return LayoutItem(
        "text",
        id=f"serein/{stem}/{option.name.replace('_', '-')}",
        title=option.title,
        keywords=cpp_keywords(option.keywords),
        cpp_name=option.cpp_name,
        placeholder=text["placeholder"],
        hidden_by=hidden_by,
    )


def number_item(item, option, number, stem, place):
    if option.ctype != "int":
        raise SchemaError(f"{place}: number inputs need an int32 option")
    rules = item.get("options", {}).get(RULES_EXTENSION, {}).get("int32", {})
    if "gte" not in rules or "lte" not in rules:
        raise SchemaError(f"{place}: number inputs need gte and lte rules")
    if not number.get("zeroLabel"):
        raise SchemaError(f"{place}: number inputs need a zero_label")
    return LayoutItem(
        "number",
        id=f"serein/{stem}/{option.name.replace('_', '-')}",
        title=option.title,
        keywords=cpp_keywords(option.keywords),
        cpp_name=option.cpp_name,
        minimum=max(1, int(rules["gte"])),
        maximum=int(rules["lte"]),
        zero_label=number["zeroLabel"],
        count_format=number.get("countFormat", ""),
        suffix=number.get("suffix", ""),
        hint=number.get("hint", ""),
    )


def choice_item(item, option, choice, stem, place):
    if option.ctype != "int":
        raise SchemaError(f"{place}: choice inputs need an int32 option")
    rules = item.get("options", {}).get(RULES_EXTENSION, {}).get("int32", {})
    labels = list(choice.get("labels", []))
    if labels:
        if (int(rules.get("gte", -1)), int(rules.get("lte", -1))) != (0, len(labels) - 1):
            raise SchemaError(f"{place}: choice labels need gte 0 and lte {len(labels) - 1}")
        values = list(range(len(labels)))
    elif rules.get("in"):
        values = [int(value) for value in rules["in"]]
    else:
        raise SchemaError(f"{place}: choice inputs need labels or an in rule")
    return LayoutItem(
        "choice",
        id=f"serein/{stem}/{option.name.replace('_', '-')}",
        title=option.title,
        keywords=cpp_keywords(option.keywords),
        cpp_name=option.cpp_name,
        values=values,
        labels=labels,
        suffix=choice.get("suffix", ""),
    )


def check_titles(pages, known):
    titles = {row.title for page in pages for row in page.rows}
    titles |= {
        item.title
        for page in pages
        for item in page.layout
        if item.kind in ("section", "note", "number", "choice", "text")
    }
    titles |= {label for page in pages for item in page.layout for label in item.labels}
    titles |= {
        label
        for page in pages
        for item in page.layout
        for label in (item.zero_label, item.count_format, item.hint, item.placeholder)
        if label
    }
    titles |= {page.subpage["title"] for page in pages if page.subpage}
    missing = sorted(title for title in titles if title not in known)
    if missing:
        raise SchemaError(f"settings rows use unknown strings: {missing}")
