import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

import check_style

GOOD = """\
#include "serein/example.h"

namespace Serein::Example {
namespace {

class Widget final {
public:
\tvoid paint();

private:
\tint _value = 0;

};

struct Plain {
\tint value = 0;
};

} // namespace

// WHY: two lines of reasoning are allowed
// when they open with the marker.
bool Check(int a, int b) {
\treturn (a > 0)
\t\t&& (b > 0); // a trailing comment
}

} // namespace Serein::Example
"""


class StyleTest(unittest.TestCase):
    def setUp(self):
        self._temp = tempfile.TemporaryDirectory()
        self.root = Path(self._temp.name)

    def tearDown(self):
        self._temp.cleanup()

    def problems(self, name, content):
        path = self.root / name
        path.parent.mkdir(parents=True, exist_ok=True)
        data = content if isinstance(content, bytes) else content.encode()
        path.write_bytes(data)
        relative = path.relative_to(self.root)
        errors = list(check_style.text_errors(relative, data))
        if relative.suffix in check_style.CPP and not errors:
            lines = data.decode().split("\n")[:-1]
            errors += list(check_style.cpp_errors(relative, lines))
        return [message for _, message in errors]

    def test_accepts_conforming_code(self):
        self.assertEqual(self.problems("serein/a.cpp", GOOD), [])

    def test_text_rules(self):
        self.assertIn(
            "starts with a UTF-8 byte order mark", self.problems("a.md", b"\xef\xbb\xbfx\n")
        )
        self.assertIn("has CR line endings", self.problems("a.md", "x\r\n"))
        self.assertIn("does not end with a newline", self.problems("a.md", "x"))
        self.assertIn("ends with empty lines", self.problems("a.md", "x\n\n"))
        self.assertIn("has trailing whitespace", self.problems("a.md", "x \n"))
        self.assertIn("is indented with a tab", self.problems("a.yml", "a:\n\tb: 1\n"))
        self.assertIn("is not UTF-8 (invalid start byte)", self.problems("a.md", b"\xff\n"))

    def test_json_rules(self):
        self.assertTrue(any("not valid JSON" in p for p in self.problems("a.json", "{\n")))
        self.assertIn(
            "is not formatted like json.dumps(indent=2)",
            self.problems("tools/serein/policy/a.json", '{"a": 1}\n'),
        )
        self.assertEqual(self.problems("tools/serein/policy/a.json", '{\n  "a": 1\n}\n'), [])
        self.assertEqual(self.problems("data/a.json", '{"a": 1}\n'), [])

    def test_cpp_layout_rules(self):
        self.assertIn(
            "is indented with spaces", self.problems("a.cpp", "void f() {\n    g();\n}\n")
        )
        self.assertIn("follows another empty line", self.problems("a.cpp", "int a;\n\n\nint b;\n"))
        self.assertIn(
            "put '&&' or '||' at the start of the next line",
            self.problems("a.cpp", "bool x = a &&\n\tb;\n"),
        )
        self.assertIn(
            "use nested namespace syntax",
            self.problems(
                "a.cpp", "namespace A {\nnamespace B {\n} // namespace B\n} // namespace A\n"
            ),
        )
        self.assertIn(
            "class with access sections needs an empty line before '};'",
            self.problems("a.h", "class A {\npublic:\n\tint x;\n};\n"),
        )
        self.assertIn(
            "[[nodiscard]] belongs on the declaration only",
            self.problems("a.cpp", "[[nodiscard]] int Foo::Bar() {\n\treturn 0;\n}\n"),
        )

    def test_banned_constructs(self):
        cases = {
            'auto s = QStringLiteral("x");\n': "QStringLiteral",
            "static_cast<void>(f());\n": "cast",
            "(void)f();\n": "cast",
            "#ifdef Q_OS_LINUX\n#endif\n": "Q_OS_WIN",
            "int *p = NULL;\n": "nullptr",
            "#ifdef _DEBUG\n#endif\n": "test harness",
        }
        for code, fragment in cases.items():
            problems = self.problems("serein/a.cpp", code)
            self.assertTrue(any(fragment in p for p in problems), (code, problems))

    def test_strings_and_comments_do_not_count(self):
        code = 'auto a = u"x &&"_q;\n// QStringLiteral(\nauto b = R"(NULL)";\n'
        self.assertEqual(self.problems("a.cpp", code), [])

    def test_comment_rationing(self):
        self.assertIn(
            "multi-line comment must open with '// WHY:'",
            self.problems("serein/a.cpp", "// one\n// two\nint a;\n"),
        )
        self.assertIn(
            "comment block is longer than three lines",
            self.problems("serein/a.cpp", "// WHY: a\n// b\n// c\n// d\nint a;\n"),
        )
        self.assertEqual(self.problems("serein/tests/a.cpp", "// one\n// two\nint a;\n"), [])

    def test_main_reports_files_from_the_command_line(self):
        (self.root / "a.cpp").write_text("int a; \n")
        self.assertEqual(check_style.main(["--root", str(self.root), "a.cpp"]), 1)
        (self.root / "a.cpp").write_text("int a;\n")
        self.assertEqual(check_style.main(["--root", str(self.root), "a.cpp"]), 0)


if __name__ == "__main__":
    unittest.main()
