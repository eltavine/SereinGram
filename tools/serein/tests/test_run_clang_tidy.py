import contextlib
import io
import json
import shlex
import sys
import tempfile
import unittest
from pathlib import Path
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

import run_clang_tidy

OWNED = "/repo/Telegram/SourceFiles/serein"

FAKE_TIDY = """
import sys

path = sys.argv[-1]
if path.endswith("dirty.cpp"):
    print(f"{path}:3:5: warning: suspicious call [bugprone-foo]")
    print(f"{path}:3:5: note: declared here")
    sys.exit(1)
if path.endswith("broken.cpp"):
    print("Error while processing " + path)
    sys.exit(1)
print("/usr/include/upstream.h:1:1: warning: noisy macro [bugprone-bar]")
"""


class OwnedTest(unittest.TestCase):
    def test_hand_written_sources_are_owned(self):
        self.assertTrue(run_clang_tidy.owned(f"{OWNED}/menu/actions.cpp"))
        self.assertTrue(run_clang_tidy.owned("C:\\repo\\Telegram\\SourceFiles\\serein\\a.cpp"))

    def test_generated_and_upstream_sources_are_not(self):
        self.assertFalse(run_clang_tidy.owned(f"{OWNED}/schema/gen/settings/menu.h"))
        self.assertFalse(run_clang_tidy.owned("/repo/Telegram/SourceFiles/history/history.cpp"))
        self.assertFalse(run_clang_tidy.owned("/repo/Telegram/lib_base/base/assertion.h"))

    def test_gen_above_the_owned_root_does_not_matter(self):
        self.assertTrue(run_clang_tidy.owned("/gen/repo/Telegram/SourceFiles/serein/a.cpp"))


class FindingsTest(unittest.TestCase):
    def test_reports_only_owned_diagnostics_and_parse_errors(self):
        output = "\n".join(
            [
                f"{OWNED}/menu/actions.cpp:10:2: warning: copy [performance-unnecessary-copy]",
                f"{OWNED}/schema/gen/a.cpp:1:1: warning: generated [bugprone-foo]",
                "/repo/Telegram/lib_base/base/assertion.h:5:1: warning: macro [bugprone-bar]",
                "/repo/Telegram/SourceFiles/h.h:2:3: error: missing [clang-diagnostic-error]",
                f"{OWNED}/menu/actions.cpp:10:2: note: expanded from here",
                "1 warning generated.",
            ]
        )
        found = list(run_clang_tidy.findings(output))
        self.assertEqual(len(found), 2)
        self.assertIn("performance-unnecessary-copy", found[0])
        self.assertIn("clang-diagnostic-error", found[1])


class SourcesTest(unittest.TestCase):
    def test_lists_owned_translation_units_once(self):
        with tempfile.TemporaryDirectory() as temp:
            database = Path(temp) / "compile_commands.json"
            entries = [
                {"directory": OWNED, "file": "menu/actions.cpp"},
                {"directory": OWNED, "file": "menu/actions.cpp"},
                {"directory": OWNED, "file": "app/system_ai.mm"},
                {"directory": OWNED, "file": "schema/gen/menu.cpp"},
                {"directory": "/repo/Telegram/SourceFiles", "file": "history/history.cpp"},
                {"directory": OWNED, "file": "menu/model.h"},
            ]
            database.write_text(json.dumps(entries), encoding="utf-8")
            self.assertEqual(
                run_clang_tidy.sources(database),
                [f"{OWNED}/app/system_ai.mm", f"{OWNED}/menu/actions.cpp"],
            )


class MainTest(unittest.TestCase):
    def setUp(self):
        self._temp = tempfile.TemporaryDirectory()
        self.root = Path(self._temp.name)
        self.tidy = self.root / "fake_tidy.py"
        self.tidy.write_text(FAKE_TIDY, encoding="utf-8")
        patcher = mock.patch.object(run_clang_tidy, "default_extra_args", return_value=[])
        patcher.start()
        self.addCleanup(patcher.stop)
        self.addCleanup(self._temp.cleanup)

    def run_with(self, files):
        entries = [{"directory": OWNED, "file": name} for name in files]
        (self.root / "compile_commands.json").write_text(json.dumps(entries), encoding="utf-8")
        command = f"{shlex.quote(sys.executable)} {shlex.quote(str(self.tidy))}"
        output = io.StringIO()
        with contextlib.redirect_stdout(output), contextlib.redirect_stderr(output):
            code = run_clang_tidy.main(["-p", str(self.root), "--clang-tidy", command, "-j", "2"])
        return code, output.getvalue()

    def test_clean_sources_pass(self):
        code, output = self.run_with(["menu/clean.cpp", "menu/other.cpp"])
        self.assertEqual(code, 0, output)
        self.assertIn("2 SereinGram translation units are clean", output)

    def test_owned_findings_fail_once_per_line(self):
        code, output = self.run_with(["menu/dirty.cpp", "menu/clean.cpp"])
        self.assertEqual(code, 1)
        self.assertEqual(output.count("[bugprone-foo]"), 1)
        self.assertIn("1 findings, 0 files failed", output)

    def test_unparsable_sources_fail(self):
        code, output = self.run_with(["menu/broken.cpp"])
        self.assertEqual(code, 1)
        self.assertIn("0 findings, 1 files failed", output)

    def test_empty_database_fails(self):
        code, output = self.run_with([])
        self.assertEqual(code, 1)
        self.assertIn("No SereinGram sources", output)


if __name__ == "__main__":
    unittest.main()
