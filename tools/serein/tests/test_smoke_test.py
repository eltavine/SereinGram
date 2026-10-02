import stat
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

import smoke_test  # noqa: E402

LAUNCHING = """#!/bin/sh
echo "Launched version: 1.0" > "$2/log.txt"
exec sleep 30
"""
CRASHING = """#!/bin/sh
echo "Launched version: 1.0" > "$2/log.txt"
exit 3
"""
SILENT = """#!/bin/sh
exec sleep 30
"""


@unittest.skipIf(sys.platform == "win32", "uses shell scripts")
class SmokeTest(unittest.TestCase):
    def setUp(self):
        self._temp = tempfile.TemporaryDirectory()
        self.dir = Path(self._temp.name)

    def tearDown(self):
        self._temp.cleanup()

    def script(self, text):
        path = self.dir / "app.sh"
        path.write_text(text)
        path.chmod(path.stat().st_mode | stat.S_IXUSR)
        return path

    def test_passes_when_the_app_keeps_running(self):
        self.assertEqual(
            smoke_test.main([str(self.script(LAUNCHING)), "--timeout", "10",
                             "--settle", "1"]), 0)

    def test_fails_when_the_app_exits(self):
        self.assertEqual(
            smoke_test.main([str(self.script(CRASHING)), "--timeout", "10",
                             "--settle", "2"]), 1)

    def test_fails_without_the_launch_line(self):
        self.assertEqual(
            smoke_test.main([str(self.script(SILENT)), "--timeout", "2",
                             "--settle", "1"]), 1)


if __name__ == "__main__":
    unittest.main()
