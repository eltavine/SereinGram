import importlib.util
import sys
import unittest
from pathlib import Path

CODEGEN = Path(__file__).resolve().parents[1] / "codegen"
sys.path.insert(0, str(CODEGEN))

import model  # noqa: E402

PAGE = {"cppNamespace": "Messages", "scope": "SCOPE_DEVICE",
        "category": "CATEGORY_MESSAGES"}


def field(name, json_name, kind="TYPE_BOOL", options=None):
    return {"name": name, "jsonName": json_name, "type": kind,
            "label": "LABEL_OPTIONAL", "options": options or {}}


def image(*fields, page=PAGE, name="serein/settings/v1/messages.proto"):
    return {"file": [{"name": name, "messageType": [{
        "name": "MessagesSettings",
        "options": {model.PAGE_EXTENSION: page},
        "field": list(fields),
    }]}]}


def only_option(*fields):
    return model.build_pages(image(*fields))[0].options[0]


class ModelTest(unittest.TestCase):
    def test_naming_conventions(self):
        option = only_option(field("seconds_in_messages", "secondsInMessages"))
        self.assertEqual(option.cpp_name, "kSecondsInMessages")
        self.assertEqual(option.key, "serein.secondsInMessages")
        self.assertEqual(option.title, "lng_serein_seconds_in_messages")
        self.assertEqual(option.scope, "Device")
        self.assertEqual(option.category, "Messages")
        self.assertEqual(option.initializer[-1], "\t0 };")

    def test_overrides_and_flags(self):
        option = only_option(field("chat_preview_lines", "chatPreviewLines", "TYPE_INT32", {
            model.FIELD_EXTENSION: {
                "cppName": "kPreviewLines", "title": "lng_x",
                "scope": "SCOPE_ACCOUNT", "refresh": ["REFRESH_DIALOG_LIST"],
                "restart": True, "defaultInt": 3,
            }}))
        self.assertEqual(option.cpp_name, "kPreviewLines")
        self.assertEqual(option.title, "lng_x")
        self.assertEqual(option.scope, "Account")
        self.assertEqual(option.fallback, "3")
        self.assertEqual(option.flags, ["RefreshDialogList", "RequiresRestart"])
        self.assertIn("Flag::RequiresRestart", option.flags_expression)

    def test_fallback_literals(self):
        self.assertEqual(model.fallback_literal("QString", {}), "QString()")
        self.assertEqual(model.fallback_literal("QByteArray", {"defaultString": '{"a":1}'}),
                         'QByteArray("{\\"a\\":1}")')
        self.assertEqual(model.fallback_literal("bool", {"defaultBool": True}), "true")

    def test_int_rules_accept_fallback_first(self):
        option = only_option(field("width", "width", "TYPE_INT32", {
            model.RULES_EXTENSION: {"int32": {"lte": 400, "gte": 50}}}))
        self.assertEqual(option.validator[1], "\treturn (value == 0)")
        self.assertEqual(option.validator[2], "\t\t|| ((value >= 50) && (value <= 400));")

    def test_in_rule(self):
        option = only_option(field("delay", "delay", "TYPE_INT32", {
            model.RULES_EXTENSION: {"int32": {"in": [0, 500]}}}))
        self.assertIn("(value == 0 || value == 500)", option.validator[2])

    def test_string_rules_need_regex(self):
        page = model.build_pages(image(field("code", "code", "TYPE_STRING", {
            model.RULES_EXTENSION: {"string": {"maxLen": "32", "pattern": "^[a-z]*$"}}})))[0]
        self.assertTrue(page.needs_regex)
        self.assertIn("value.toUcs4().size() <= 32", page.options[0].validator[2])

    def test_custom_validator_is_declared(self):
        page = model.build_pages(image(field("sort", "sort", "TYPE_INT32", {
            model.FIELD_EXTENSION: {"validator": "ValidSort"}})))[0]
        self.assertEqual(page.custom_validators, [("ValidSort", "int")])
        self.assertEqual(page.options[0].initializer[-1], "\t&ValidSort };")

    def test_unsupported_rule_fails_closed(self):
        with self.assertRaises(model.SchemaError):
            only_option(field("x", "x", "TYPE_INT32", {
                model.RULES_EXTENSION: {"int32": {"example": [1]}}}))
        with self.assertRaises(model.SchemaError):
            only_option(field("x", "x", "TYPE_INT32", {
                model.RULES_EXTENSION: {"ignore": "IGNORE_ALWAYS"}}))

    def test_rules_and_custom_validator_conflict(self):
        with self.assertRaises(model.SchemaError):
            only_option(field("x", "x", "TYPE_INT32", {
                model.FIELD_EXTENSION: {"validator": "Valid"},
                model.RULES_EXTENSION: {"int32": {"gte": 0}}}))

    def test_rejects_repeated_and_unknown_types(self):
        repeated = field("x", "x")
        repeated["label"] = "LABEL_REPEATED"
        with self.assertRaises(model.SchemaError):
            only_option(repeated)
        with self.assertRaises(model.SchemaError):
            only_option(field("x", "x", "TYPE_DOUBLE"))

    def test_rejects_duplicate_keys_across_pages(self):
        duplicate = image(field("x", "x"))
        duplicate["file"].append(image(field("x", "x"),
                                       name="serein/settings/v1/other.proto")["file"][0])
        with self.assertRaises(model.SchemaError):
            model.build_pages(duplicate)

    def test_ignores_files_outside_settings(self):
        self.assertEqual(model.build_pages(image(field("x", "x"), name="serein/other.proto")), [])


@unittest.skipUnless(importlib.util.find_spec("jinja2"), "jinja2 is not installed")
class RenderTest(unittest.TestCase):
    def test_renders_header(self):
        import generate
        output = generate.render(image(field("seconds_in_messages", "secondsInMessages")))
        text = output["settings/messages.h"]
        self.assertIn("namespace Serein::Messages {", text)
        self.assertIn("inline constexpr auto kSecondsInMessages = Option<bool>{", text)
        self.assertIn("\tExpects(registry.Add(kSecondsInMessages));", text)


if __name__ == "__main__":
    unittest.main()
