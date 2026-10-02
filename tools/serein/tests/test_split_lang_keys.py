import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

import split_lang_keys

GENERATED = """\
namespace Lang {

ushort GetKeyIndex(QLatin1String key) {
\tauto size = key.size();
\tauto data = key.data();
\tif (size >= 9 && !memcmp(data + 0, "lng_", 4)) {
\t\tswitch (data[4]) {
\t\tcase 'b':
\t\t\tswitch (data[5]) {
\t\t\tcase 'o':
\t\t\t\treturn (size == 9) ? 2 : kKeysCount;
\t\t\tbreak;
\t\t\t}
\t\tbreak;
\t\tcase 'a':
\t\t\tif (!memcmp(data + 5, "bout", 4)) {
\t\t\t\treturn (size == 9) ? 1 : kKeysCount;
\t\t\t}
\t\tbreak;
\t\t}
\t}

\treturn kKeysCount;
}

bool IsTagReplaced(ushort key, ushort tag) {
\treturn false;
}

} // namespace Lang
"""


class SplitLangKeysTest(unittest.TestCase):
    def test_moves_each_letter_into_a_function(self):
        result = split_lang_keys.split(GENERATED)
        self.assertIn("ushort GetKeyIndexPart0(", result)
        self.assertIn("ushort GetKeyIndexPart1(", result)
        self.assertIn("\t\tcase 'b': return GetKeyIndexPart0(size, data);", result)
        self.assertIn("\t\tcase 'a': return GetKeyIndexPart1(size, data);", result)
        self.assertIn('if (!memcmp(data + 5, "bout", 4)) {', result)
        self.assertEqual(result.count("\treturn kKeysCount;"), 3)
        self.assertLess(
            result.index("} // namespace\n"), result.index("ushort GetKeyIndex(QLatin1String")
        )
        self.assertTrue(result.endswith("} // namespace Lang\n"))
        self.assertIn("bool IsTagReplaced", result)

    def test_keeps_nested_breaks_inside_the_parts(self):
        result = split_lang_keys.split(GENERATED)
        part = result[
            result.index("ushort GetKeyIndexPart0(") : result.index("ushort GetKeyIndexPart1(")
        ]
        self.assertIn("\t\t\tbreak;", part)
        self.assertNotIn("\n\t\tbreak;", part)

    def test_accepts_windows_line_endings(self):
        result = split_lang_keys.split(GENERATED.replace("\n", "\r\n"))
        self.assertIn("return GetKeyIndexPart1(size, data);", result)
        self.assertNotIn("\r", result)

    def test_is_idempotent(self):
        once = split_lang_keys.split(GENERATED)
        self.assertEqual(split_lang_keys.split(once), once)

    def test_copies_unexpected_shapes(self):
        missing = GENERATED.replace("ushort GetKeyIndex(", "ushort Other(")
        self.assertEqual(split_lang_keys.split(missing), missing)
        open_case = GENERATED.replace("\t\tbreak;\n\t\t}\n", "\t\t}\n")
        self.assertEqual(split_lang_keys.split(open_case), open_case)

    def test_main_writes_the_output(self):
        with tempfile.TemporaryDirectory() as temp:
            source = Path(temp) / "lang_auto.cpp"
            target = Path(temp) / "serein_lang_auto.cpp"
            source.write_text(GENERATED, encoding="utf-8")
            self.assertEqual(split_lang_keys.main([str(source), str(target)]), 0)
            self.assertIn("GetKeyIndexPart1", target.read_text(encoding="utf-8"))
            self.assertEqual(split_lang_keys.main([str(source)]), 2)


if __name__ == "__main__":
    unittest.main()
