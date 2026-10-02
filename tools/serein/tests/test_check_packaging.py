import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

import check_packaging

PINS = {
    "tde2e": "1" * 40,
    "webrtc": "2" * 40,
    "tlottie": "3" * 40,
    "patches": "4" * 40,
}
STALE = "9" * 40


class CheckPackagingTest(unittest.TestCase):
    def setUp(self):
        self._temp = tempfile.TemporaryDirectory()
        self.root = Path(self._temp.name)
        self.write()

    def tearDown(self):
        self._temp.cleanup()

    def write(self, td=None, patches_second=None, qt="6.11.2"):
        snap = ["parts:"]
        for part, commit in PINS.items():
            snap += [f"  {part}:", "    source-depth: 1", f"    source-commit: {commit}", ""]
        snap += ["  qt:", "    source-tag: v6.11.2", "    plugin: cmake", ""]
        flatpak = [f"        url: https://download.qt.io/qt-everywhere-src-{qt}.tar.xz"]
        for part, url in check_packaging.FLATPAK_SOURCES.items():
            flatpak += ["      - type: git", f"        url: {url}", f"        commit: {PINS[part]}"]
        flatpak += [
            "      - type: git",
            f"        url: {check_packaging.FLATPAK_SOURCES['patches']}",
            f"        commit: {patches_second or PINS['patches']}",
        ]
        files = {
            check_packaging.SNAP: "\n".join(snap) + "\n",
            check_packaging.FLATPAK: "\n".join(flatpak) + "\n",
            check_packaging.PKGBUILD: f"_td_commit={td or PINS['tde2e']}\n",
        }
        for name, text in files.items():
            path = self.root / name
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text(text, encoding="utf-8")

    def test_accepts_matching_pins(self):
        self.assertEqual(check_packaging.problems(self.root), [])

    def test_reports_stale_pkgbuild_commit(self):
        self.write(td=STALE)
        found = check_packaging.problems(self.root)
        self.assertEqual(len(found), 1)
        self.assertIn("pins tdlib 9999999999", found[0])

    def test_reports_every_stale_flatpak_source(self):
        self.write(patches_second=STALE)
        found = check_packaging.problems(self.root)
        self.assertEqual(len(found), 1)
        self.assertIn("patches.git at 9999999999", found[0])

    def test_reports_qt_version_drift(self):
        self.write(qt="6.11.1")
        found = check_packaging.problems(self.root)
        self.assertEqual(
            found,
            [
                f"{check_packaging.FLATPAK} builds Qt 6.11.1, but "
                f"{check_packaging.SNAP} uses v6.11.2."
            ],
        )

    def test_update_moves_stale_pins(self):
        self.write(td=STALE, patches_second=STALE)
        self.assertEqual(
            check_packaging.update(self.root),
            [
                f"tde2e {STALE[:10]} -> {PINS['tde2e'][:10]}",
                f"patches {STALE[:10]} -> {PINS['patches'][:10]}",
            ],
        )
        self.assertEqual(check_packaging.problems(self.root), [])
        self.assertEqual(check_packaging.update(self.root), [])

    def test_update_skips_missing_recipes(self):
        (self.root / check_packaging.FLATPAK).unlink()
        self.assertEqual(check_packaging.update(self.root), [])

    def test_repository_is_consistent(self):
        self.assertEqual(check_packaging.problems(check_packaging.ROOT), [])


if __name__ == "__main__":
    unittest.main()
