import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

import check_packaging  # noqa: E402

FIRST = "1" * 40
SECOND = "2" * 40


class CheckPackagingTest(unittest.TestCase):
    def setUp(self):
        self._temp = tempfile.TemporaryDirectory()
        self.root = Path(self._temp.name)

    def tearDown(self):
        self._temp.cleanup()

    def write(self, snap, arch):
        (self.root / "snap").mkdir(exist_ok=True)
        (self.root / "packaging/arch").mkdir(parents=True, exist_ok=True)
        (self.root / check_packaging.SNAP).write_text(
            "parts:\n"
            "  webrtc:\n"
            f"    source-commit: {SECOND}\n"
            "  tde2e:\n"
            "    source: https://github.com/tdlib/td.git\n"
            "    source-depth: 1\n"
            f"    source-commit: {snap}\n"
            "    plugin: cmake\n",
            encoding="utf-8")
        (self.root / check_packaging.PKGBUILD).write_text(
            f"pkgname=x\n_td_commit={arch}\n", encoding="utf-8")

    def test_accepts_matching_commit(self):
        self.write(FIRST, FIRST)
        self.assertEqual(check_packaging.problems(self.root), [])

    def test_reports_stale_commit(self):
        self.write(FIRST, SECOND)
        found = check_packaging.problems(self.root)
        self.assertEqual(len(found), 1)
        self.assertIn("pins tdlib 2222222222", found[0])
        self.assertIn("tde2e from 1111111111", found[0])

    def test_reports_missing_pin(self):
        self.write(FIRST, "main")
        found = check_packaging.problems(self.root)
        self.assertEqual(len(found), 1)
        self.assertIn("no 40-character _td_commit", found[0])

    def test_repository_is_consistent(self):
        self.assertEqual(check_packaging.problems(check_packaging.ROOT), [])


if __name__ == "__main__":
    unittest.main()
