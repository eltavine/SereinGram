import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "codegen"))

import codec_model
from model import RULES_EXTENSION, SchemaError


def field(name, json_name, kind="TYPE_STRING", **extra):
    return {"name": name, "jsonName": json_name, "type": kind, "label": "LABEL_OPTIONAL", **extra}


def image(messages, enums=(), options=None):
    return {
        "file": [
            {
                "name": "serein/history/v1/record.proto",
                "package": "serein.history.v1",
                "options": options
                if options is not None
                else {codec_model.FILE_EXTENSION: {"cppNamespace": "History"}},
                "enumType": list(enums),
                "messageType": list(messages),
            }
        ]
    }


def one_file(*args, **kwargs):
    return codec_model.build_files(image(*args, **kwargs))[0]


def map_message(value_type="TYPE_STRING", key_type="TYPE_STRING", rules=None, nested_extra=None):
    entry = {
        "name": "StatesEntry",
        "options": {"mapEntry": True},
        "field": [
            {"name": "key", "number": 1, "type": key_type, "jsonName": "key"},
            {"name": "value", "number": 2, "type": value_type, "jsonName": "value"},
        ],
    }
    states = field(
        "states",
        "states",
        "TYPE_MESSAGE",
        label="LABEL_REPEATED",
        typeName=".serein.history.v1.Record.StatesEntry",
    )
    if rules:
        states["options"] = {RULES_EXTENSION: rules}
    return {"name": "Record", "field": [states], "nestedType": [entry, *(nested_extra or [])]}


class CodecModelTest(unittest.TestCase):
    def test_enum_values_drop_the_prefix(self):
        enum = {
            "name": "RecordKind",
            "value": [
                {"name": "RECORD_KIND_UNSPECIFIED"},
                {"name": "RECORD_KIND_DELETED", "number": 1},
            ],
        }
        built = one_file([], [enum]).enums[0]
        self.assertEqual([value.cpp for value in built.values], ["Unspecified", "Deleted"])
        self.assertEqual(built.values[1].json, "RECORD_KIND_DELETED")

    def test_enum_prefix_is_required(self):
        enum = {"name": "RecordKind", "value": [{"name": "KIND_UNSPECIFIED"}]}
        with self.assertRaises(SchemaError):
            one_file([], [enum])

    def test_field_types_and_defaults(self):
        message = {
            "name": "Record",
            "field": [
                field("peer_id", "peerId", "TYPE_INT64"),
                field("tags", "tags", label="LABEL_REPEATED"),
                field("note", "note", proto3Optional=True, oneofIndex=0),
            ],
        }
        fields = one_file([message]).messages[0].fields
        self.assertEqual(
            [(f.cpp_type, f.default) for f in fields],
            [("qint64", "0"), ("std::vector<QString>", ""), ("std::optional<QString>", "")],
        )

    def test_rules_become_checks(self):
        message = {
            "name": "Record",
            "field": [
                field("id", "id", options={RULES_EXTENSION: {"string": {"uuid": True}}}),
                field(
                    "names",
                    "names",
                    label="LABEL_REPEATED",
                    options={
                        RULES_EXTENSION: {
                            "repeated": {
                                "maxItems": "4",
                                "unique": True,
                                "items": {"string": {"maxLen": "8"}},
                            }
                        }
                    },
                ),
            ],
        }
        checks = "\n".join(one_file([message]).messages[0].checks)
        self.assertIn("Codec::IsUuid(value.id)", checks)
        self.assertIn("qsizetype(value.names.size()) <= 4", checks)
        self.assertIn("Codec::Unique(value.names)", checks)
        self.assertIn("item.toUcs4().size() <= 8", checks)

    def test_map_fields(self):
        built = one_file(
            [
                map_message(
                    rules={
                        "map": {
                            "maxPairs": 8,
                            "keys": {"string": {"pattern": "^E[0-9]{2}$"}},
                            "values": {"string": {"in": ["show", "hide"]}},
                        }
                    }
                )
            ]
        ).messages[0]
        self.assertEqual(built.fields[0].cpp_type, "std::map<QString, QString>")
        checks = "\n".join(built.checks)
        self.assertIn("qsizetype(value.states.size()) <= 8", checks)
        self.assertIn("for (const auto &[key, item] : value.states) {", checks)
        self.assertIn('Codec::Matches(key, QString::fromUtf8("^E[0-9]{2}$"))', checks)
        self.assertIn('item == QString::fromUtf8("show")', checks)
        self.assertIn("Codec::Entry(Codec::Child(path", checks)

    def test_map_keys_must_be_strings(self):
        with self.assertRaisesRegex(SchemaError, "map keys must be strings"):
            one_file([map_message(key_type="TYPE_INT32")])

    def test_other_nested_types_are_rejected(self):
        with self.assertRaisesRegex(SchemaError, "nested types"):
            one_file([map_message(nested_extra=[{"name": "Inner"}])])

    def test_string_in_lists(self):
        message = {
            "name": "Record",
            "field": [
                field(
                    "action",
                    "action",
                    options={RULES_EXTENSION: {"string": {"in": ["mask", "hide"]}}},
                )
            ],
        }
        checks = "\n".join(one_file([message]).messages[0].checks)
        self.assertIn(
            '(value.action == QString::fromUtf8("mask") || '
            'value.action == QString::fromUtf8("hide"))',
            checks,
        )

    def test_not_in_uses_inequalities(self):
        message = {
            "name": "Record",
            "field": [
                field(
                    "peer_id",
                    "peerId",
                    "TYPE_INT64",
                    options={RULES_EXTENSION: {"int64": {"notIn": ["0"]}}},
                )
            ],
        }
        self.assertIn("value.peerId != 0", "\n".join(one_file([message]).messages[0].checks))

    def test_unsupported_rules_fail_closed(self):
        message = {
            "name": "Record",
            "field": [field("id", "id", options={RULES_EXTENSION: {"string": {"email": True}}})],
        }
        with self.assertRaises(SchemaError):
            one_file([message])

    def test_messages_are_ordered_by_dependency(self):
        outer = {
            "name": "Outer",
            "field": [field("inner", "inner", "TYPE_MESSAGE", typeName=".serein.history.v1.Inner")],
        }
        inner = {"name": "Inner", "field": [field("x", "x")]}
        names = [message.name for message in one_file([outer, inner]).messages]
        self.assertEqual(names, ["Inner", "Outer"])

    def test_reference_cycles_are_rejected(self):
        a = {
            "name": "A",
            "field": [field("b", "b", "TYPE_MESSAGE", typeName=".serein.history.v1.B")],
        }
        b = {
            "name": "B",
            "field": [field("a", "a", "TYPE_MESSAGE", typeName=".serein.history.v1.A")],
        }
        with self.assertRaises(SchemaError):
            one_file([a, b])

    def test_types_from_other_files_are_rejected(self):
        message = {
            "name": "Record",
            "field": [
                field("stamp", "stamp", "TYPE_MESSAGE", typeName=".google.protobuf.Timestamp")
            ],
        }
        with self.assertRaises(SchemaError):
            one_file([message])

    def test_documents_reserve_version(self):
        message = {
            "name": "Record",
            "options": {codec_model.DOCUMENT_EXTENSION: {"version": 1}},
            "field": [field("version", "version", "TYPE_INT32")],
        }
        with self.assertRaises(SchemaError):
            one_file([message])

    def test_files_without_options_are_skipped(self):
        self.assertEqual(codec_model.build_files(image([], options={})), [])


if __name__ == "__main__":
    unittest.main()
