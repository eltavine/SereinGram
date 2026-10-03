import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

import check_settings_index as check


def lines(source):
    return [problem.split(":")[1] for problem in check.problems("page.cpp", source)]


class SettingsIndexTest(unittest.TestCase):
    def test_building_code_must_not_dereference_the_window(self):
        source = """
void AddRow(SectionBuilder &builder) {
    const auto controller = builder.controller();
    const auto session = &controller->session();
}
"""
        self.assertEqual(lines(source), ["4"])

    def test_callbacks_that_capture_by_value_may_use_it(self):
        source = """
void AddRow(SectionBuilder &builder, QString id) {
    const auto controller = builder.controller();
    builder.addButton({
        .label = Value() | rpl::map([=](int) { return controller->title(); }),
        .onClick = [=] { controller->show(Box(PageBox)); },
    });
    button->clicks() | rpl::on_next(crl::guard(controller, [=] {
        controller->window().activate();
    }), lifetime);
}
"""
        self.assertEqual(lines(source), [])

    def test_rows_built_by_reference_run_while_building(self):
        source = """
const auto kMeta = BuildHelper({}, [](SectionBuilder &builder) {
    const auto controller = builder.controller();
    AddLayout(builder, {
        .folder = [&] {
            const auto session = &controller->session();
            builder.addButton({ .onClick = [=] { controller->show(Box()); } });
        },
    });
});
"""
        self.assertEqual(lines(source), ["6"])

    def test_by_reference_code_inside_a_callback_is_deferred(self):
        source = """
void AddRow(SectionBuilder &builder) {
    const auto controller = builder.controller();
    builder.addButton({ .onClick = [=] {
        const auto apply = [&] { controller->show(Box()); };
        apply();
    } });
}
"""
        self.assertEqual(lines(source), [])

    def test_direct_and_container_dereferences_are_found(self):
        source = """
void AddRow(::Settings::Builder::SectionBuilder &builder) {
    builder.controller()->show(Box());
    const auto container = builder.container();
    container->add(object_ptr<Ui::FlatLabel>(container, text));
}
"""
        self.assertEqual(lines(source), ["3", "5"])

    def test_comments_strings_and_other_functions_are_ignored(self):
        source = """
// builder.controller()->show(Box());
void ShowPage(not_null<Window::SessionController*> controller) {
    const auto session = &controller->session();
}
void AddRow(SectionBuilder &builder) {
    const auto controller = builder.controller();
    const auto text = u"controller->session()"_q; /* controller-> */
}
"""
        self.assertEqual(lines(source), [])

    def test_main_reports_problems_with_their_paths(self):
        with tempfile.TemporaryDirectory() as temp:
            page = Path(temp) / check.SOURCES / "settings" / "page.cpp"
            page.parent.mkdir(parents=True)
            page.write_text(
                "void AddRow(SectionBuilder &builder) {\n\tbuilder.controller()->show(Box());\n}\n",
                encoding="utf-8",
            )
            self.assertEqual(check.main([temp]), 1)
            page.write_text("void AddRow(SectionBuilder &builder) {}\n", encoding="utf-8")
            self.assertEqual(check.main([temp]), 0)


if __name__ == "__main__":
    unittest.main()
