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

    def test_string_rules_use_the_codec_matcher(self):
        page = model.build_pages(image(field("code", "code", "TYPE_STRING", {
            model.RULES_EXTENSION: {"string": {"maxLen": "32", "pattern": "^[a-z]*$"}}})))[0]
        self.assertTrue(page.needs_codec)
        self.assertIn("Codec::Matches(value", page.options[0].validator[2])
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

    def test_toggle_rows(self):
        page = model.build_pages(image(
            field("show_seconds", "showSeconds", options={
                model.FIELD_EXTENSION: {"keywords": ["time", "GIF \"x\""]}}),
            field("secret_flag", "secretFlag", options={
                model.FIELD_EXTENSION: {"hidden": True}}),
            field("drawn_by_hand", "drawnByHand", options={
                model.FIELD_EXTENSION: {"customUi": True}}),
            field("preview_lines", "previewLines", "TYPE_INT32"),
        ))[0]
        self.assertEqual(page.rows_header, "messages_rows.h")
        self.assertEqual(len(page.rows), 1)
        row = page.rows[0]
        self.assertEqual(row.cpp_name, "kShowSeconds")
        self.assertEqual(row.title, "lng_serein_show_seconds")
        self.assertEqual(row.id, "serein/messages/show-seconds")
        self.assertEqual(row.keywords, 'u"time"_q, u"GIF \\"x\\""_q')

    def test_layout_places_sections_customs_and_notes(self):
        page = model.build_pages(image(
            field("hide_all", "hideAll", options={model.FIELD_EXTENSION: {
                "section": {"title": "lng_s", "id": "group", "keywords": ["k"]}}}),
            field("hide_some", "hideSome", options={model.FIELD_EXTENSION: {
                "disabledBy": "hide_all", "note": "lng_n"}}),
            field("preview_lines", "previewLines", "TYPE_INT32"),
            field("secret_flag", "secretFlag", options={
                model.FIELD_EXTENSION: {"hidden": True}}),
        ))[0]
        self.assertEqual([item.kind for item in page.layout],
                         ["section", "toggle", "toggle", "note", "custom"])
        self.assertEqual(page.layout[0].id, "serein/messages/group")
        self.assertEqual(page.layout[0].keywords, 'u"k"_q')
        self.assertEqual(page.rows[1].disabled_by, "kHideAll")
        self.assertEqual(page.customs, ["previewLines"])
        self.assertEqual(page.rows_header, "messages_rows.h")

    def test_number_inputs_use_rules_and_labels(self):
        page = model.build_pages(image(field("keep_days", "keepDays", "TYPE_INT32", {
            model.FIELD_EXTENSION: {"number": {
                "zeroLabel": "lng_forever", "countFormat": "lng_days"}},
            model.RULES_EXTENSION: {"int32": {"gte": 0, "lte": 365}},
        })))[0]
        self.assertEqual(page.customs, [])
        item = page.layout[0]
        self.assertEqual((item.kind, item.cpp_name, item.minimum, item.maximum),
                         ("number", "kKeepDays", 1, 365))
        self.assertEqual((item.zero_label, item.count_format), ("lng_forever", "lng_days"))
        self.assertEqual(item.id, "serein/messages/keep-days")

    def test_number_inputs_are_validated(self):
        cases = {
            "int32": ({"zeroLabel": "lng_z"}, {"int32": {"gte": 0}}, "gte and lte"),
            "label": ({}, {"int32": {"gte": 0, "lte": 5}}, "zero_label"),
        }
        for name, (number, rules, message) in cases.items():
            with self.subTest(name), self.assertRaisesRegex(model.SchemaError, message):
                model.build_pages(image(field("keep_days", "keepDays", "TYPE_INT32", {
                    model.FIELD_EXTENSION: {"number": number},
                    model.RULES_EXTENSION: rules,
                })))
        with self.assertRaisesRegex(model.SchemaError, "int32 option"):
            model.build_pages(image(field("flag", "flag", options={
                model.FIELD_EXTENSION: {"number": {"zeroLabel": "lng_z"}}})))

    def test_choice_inputs(self):
        labeled = model.build_pages(image(field("mode", "mode", "TYPE_INT32", {
            model.FIELD_EXTENSION: {"choice": {"labels": ["lng_a", "lng_b"]}},
            model.RULES_EXTENSION: {"int32": {"gte": 0, "lte": 1}},
        })))[0].layout[0]
        self.assertEqual((labeled.kind, labeled.values, labeled.labels),
                         ("choice", [0, 1], ["lng_a", "lng_b"]))
        suffixed = model.build_pages(image(field("scale", "scale", "TYPE_INT32", {
            model.FIELD_EXTENSION: {"choice": {"suffix": "%"}},
            model.RULES_EXTENSION: {"int32": {"in": [50, 100]}},
        })))[0].layout[0]
        self.assertEqual((suffixed.values, suffixed.suffix), ([50, 100], "%"))

    def test_choice_inputs_are_validated(self):
        with self.assertRaisesRegex(model.SchemaError, "gte 0 and lte 1"):
            model.build_pages(image(field("mode", "mode", "TYPE_INT32", {
                model.FIELD_EXTENSION: {"choice": {"labels": ["lng_a", "lng_b"]}},
                model.RULES_EXTENSION: {"int32": {"gte": 0, "lte": 2}},
            })))
        with self.assertRaisesRegex(model.SchemaError, "labels or an in rule"):
            model.build_pages(image(field("mode", "mode", "TYPE_INT32", {
                model.FIELD_EXTENSION: {"choice": {}},
                model.RULES_EXTENSION: {"int32": {"gte": 0, "lte": 2}},
            })))

    def test_text_inputs(self):
        page = model.build_pages(image(
            field("hide_mark", "hideMark"),
            field("mark", "mark", "TYPE_STRING", {
                model.FIELD_EXTENSION: {
                    "text": {"placeholder": "lng_edited"},
                    "disabledBy": "hide_mark",
                    "keywords": ["label"],
                }}),
        ))[0]
        self.assertEqual(page.customs, [])
        item = page.layout[1]
        self.assertEqual((item.kind, item.cpp_name, item.placeholder, item.hidden_by),
                         ("text", "kMark", "lng_edited", "kHideMark"))
        self.assertEqual(item.id, "serein/messages/mark")

    def test_text_inputs_are_validated(self):
        with self.assertRaisesRegex(model.SchemaError, "string option"):
            model.build_pages(image(field("flag", "flag", options={
                model.FIELD_EXTENSION: {"text": {"placeholder": "lng_x"}}})))
        with self.assertRaisesRegex(model.SchemaError, "placeholder"):
            model.build_pages(image(field("mark", "mark", "TYPE_STRING", {
                model.FIELD_EXTENSION: {"text": {}}})))
        with self.assertRaisesRegex(model.SchemaError, "disabled_by 'missing'"):
            model.build_pages(image(field("mark", "mark", "TYPE_STRING", {
                model.FIELD_EXTENSION: {
                    "text": {"placeholder": "lng_x"}, "disabledBy": "missing"}})))

    def test_page_without_visible_options_has_no_rows_header(self):
        page = model.build_pages(image(field("secret_flag", "secretFlag", options={
            model.FIELD_EXTENSION: {"hidden": True}})))[0]
        self.assertEqual((page.rows_header, page.layout), ("", []))

    def test_disabled_by_must_name_a_visible_toggle(self):
        for target in ("missing", "preview_lines", "hide_some"):
            with self.subTest(target=target), self.assertRaisesRegex(
                    model.SchemaError, "disabled_by"):
                model.build_pages(image(
                    field("preview_lines", "previewLines", "TYPE_INT32"),
                    field("hide_some", "hideSome", options={model.FIELD_EXTENSION: {
                        "disabledBy": target}}),
                ))

    def test_section_needs_title_and_id(self):
        with self.assertRaisesRegex(model.SchemaError, "section 'id' is required"):
            model.build_pages(image(field("hide_all", "hideAll", options={
                model.FIELD_EXTENSION: {"section": {"title": "lng_s"}}})))

    def test_rejects_duplicate_row_ids(self):
        first = image(field("show_seconds", "showSeconds"))
        second = image(field("show_seconds", "otherKey"), name="serein/settings/v2/messages.proto")
        first["file"] += second["file"]
        with self.assertRaisesRegex(model.SchemaError, "duplicate settings row ids"):
            model.build_pages(first)

    def test_unknown_titles_fail(self):
        pages = model.build_pages(image(field("show_seconds", "showSeconds", options={
            model.FIELD_EXTENSION: {"note": "lng_note"}})))
        model.check_titles(pages, {"lng_serein_show_seconds", "lng_note"})
        with self.assertRaisesRegex(model.SchemaError, "lng_serein_show_seconds"):
            model.check_titles(pages, {"lng_note"})
        with self.assertRaisesRegex(model.SchemaError, "lng_note"):
            model.check_titles(pages, {"lng_serein_show_seconds"})


@unittest.skipUnless(importlib.util.find_spec("jinja2"), "jinja2 is not installed")
class RenderTest(unittest.TestCase):
    def test_renders_header(self):
        import generate
        output = generate.render(
            image(field("seconds_in_messages", "secondsInMessages")),
            known_strings={"lng_serein_seconds_in_messages"})
        text = output["schema/gen/settings/messages.h"]
        self.assertIn("namespace Serein::Messages {", text)
        self.assertIn("inline constexpr auto kSecondsInMessages = Option<bool>{", text)
        self.assertIn("\tExpects(registry.Add(kSecondsInMessages));", text)
        rows = output["settings/gen/messages_rows.h"]
        self.assertIn('#include "serein/schema/gen/settings/messages.h"', rows)
        self.assertIn("std::array<ToggleRow, 1>", rows)
        self.assertIn("\t\t&kSecondsInMessages,\n\t\ttr::lng_serein_seconds_in_messages,", rows)
        self.assertIn('u"serein/messages/seconds-in-messages"_q', rows)
        self.assertIn("\tAddToggle(builder, kToggleRows[0]);", rows)
        self.assertIn("\t\t::Settings::Builder::SectionBuilder &builder) {", rows)
        self.assertNotIn("CustomRows", rows)

    def test_renders_hook_facade(self):
        import generate
        output = generate.render(image(
            field("seconds_in_messages", "secondsInMessages"),
            field("preview_lines", "previewLines", "TYPE_INT32", {
                model.FIELD_EXTENSION: {"cppName": "kLines", "scope": "SCOPE_ACCOUNT"}}),
        ), known_strings={"lng_serein_seconds_in_messages"})
        header = output["hooks/gen/messages.h"]
        self.assertIn("namespace Serein::Hooks::Messages {", header)
        self.assertIn("[[nodiscard]] bool SecondsInMessages();", header)
        self.assertIn("[[nodiscard]] rpl::producer<bool> SecondsInMessagesValue();", header)
        self.assertIn("[[nodiscard]] int Lines(gsl::not_null<Main::Session*> session);", header)
        self.assertIn("class Session;", header)
        self.assertNotIn("serein/core", header)
        source = output["hooks/gen/messages.cpp"]
        self.assertIn("\treturn ForDevice().Get(Serein::Messages::kSecondsInMessages);", source)
        self.assertIn("\treturn ForAccount(session).Value(Serein::Messages::kLines);", source)
        sources = output["schema/gen/sources.cmake"]
        self.assertIn("set(serein_generated_hook_sources\n    serein/hooks/gen/messages.cpp\n)", sources)

    def test_renders_number_inputs(self):
        import generate
        output = generate.render(image(field("keep_days", "keepDays", "TYPE_INT32", {
            model.FIELD_EXTENSION: {"number": {
                "zeroLabel": "lng_forever", "countFormat": "lng_days"}},
            model.RULES_EXTENSION: {"int32": {"gte": 0, "lte": 365}},
        })), known_strings={"lng_serein_keep_days", "lng_forever", "lng_days"})
        rows = output["settings/gen/messages_rows.h"]
        self.assertIn("\tAddNumber(builder, {\n\t\t.option = &kKeepDays,", rows)
        self.assertIn("\t\t.maximum = 365,", rows)
        self.assertIn("return tr::lng_days(tr::now, lt_count, value);", rows)
        self.assertNotIn("CustomRows", rows)

    def test_renders_choice_inputs(self):
        import generate
        output = generate.render(image(field("mode", "mode", "TYPE_INT32", {
            model.FIELD_EXTENSION: {"choice": {"labels": ["lng_a", "lng_b"]}},
            model.RULES_EXTENSION: {"int32": {"gte": 0, "lte": 1}},
        })), known_strings={"lng_serein_mode", "lng_a", "lng_b"})
        rows = output["settings/gen/messages_rows.h"]
        self.assertIn("\t\t.values = { 0, 1 },", rows)
        self.assertIn("\t\t.labels = { tr::lng_a, tr::lng_b },", rows)

    def test_renders_text_inputs(self):
        import generate
        output = generate.render(image(field("mark", "mark", "TYPE_STRING", {
            model.FIELD_EXTENSION: {"text": {"placeholder": "lng_edited"}},
        })), known_strings={"lng_serein_mark", "lng_edited"})
        rows = output["settings/gen/messages_rows.h"]
        self.assertIn("\tAddText(builder, {\n\t\t.option = &kMark,", rows)
        self.assertIn("\t\t.placeholder = tr::lng_edited,", rows)
        self.assertNotIn(".hiddenBy", rows)
        with self.assertRaisesRegex(model.SchemaError, "lng_edited"):
            generate.render(image(field("mark", "mark", "TYPE_STRING", {
                model.FIELD_EXTENSION: {"text": {"placeholder": "lng_edited"}},
            })), known_strings={"lng_serein_mark"})

    def test_subpage_needs_title_and_style_icon(self):
        for subpage, error in (
                ({"icon": "menuIconLock"}, "subpage 'title' is required"),
                ({"title": "lng_s"}, "subpage 'icon' is required"),
                ({"title": "lng_s", "icon": "st::menuIconLock"}, "style name")):
            with self.subTest(subpage=subpage), self.assertRaisesRegex(
                    model.SchemaError, error):
                model.build_pages(image(field("hide_all", "hideAll"),
                                        page={**PAGE, "subpage": subpage}))

    def test_renders_subpage_button(self):
        import generate
        page = {**PAGE, "subpage": {
            "title": "lng_page", "icon": "menuIconLock", "keywords": ["ghost"]}}
        output = generate.render(image(field("hide_all", "hideAll"), page=page),
                                 known_strings={"lng_serein_hide_all", "lng_page"})
        rows = output["settings/gen/messages_rows.h"]
        self.assertIn('#include "styles/style_menu_icons.h"', rows)
        self.assertIn("inline constexpr auto kSubpageTitle = &tr::lng_page;", rows)
        self.assertIn("inline const auto kSubpageIcon = &st::menuIconLock;", rows)
        self.assertIn("		.targetSection = section,", rows)
        self.assertIn('		.keywords = { u"ghost"_q },', rows)
        with self.assertRaisesRegex(model.SchemaError, "lng_page"):
            generate.render(image(field("hide_all", "hideAll"), page=page),
                            known_strings={"lng_serein_hide_all"})
        plain = generate.render(image(field("hide_all", "hideAll")),
                                known_strings={"lng_serein_hide_all"})
        self.assertNotIn("AddSubpageButton", plain["settings/gen/messages_rows.h"])

    def test_renders_custom_rows(self):
        import generate
        output = generate.render(
            image(field("preview_lines", "previewLines", "TYPE_INT32")),
            known_strings=set())
        rows = output["settings/gen/messages_rows.h"]
        self.assertNotIn("kToggleRows", rows)
        self.assertIn("struct CustomRows {\n\tCustomRow previewLines;\n};", rows)
        self.assertIn("\t\tconst CustomRows &custom) {\n\tcustom.previewLines();", rows)


if __name__ == "__main__":
    unittest.main()
