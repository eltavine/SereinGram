import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

SCRIPT = Path(__file__).resolve().parents[1] / "retry.sh"
FLAKY = """\
count=$(($(cat "$1" 2>/dev/null || echo 0) + 1))
echo "$count" > "$1"
[ "$count" -gt "$2" ] || exit "$3"
"""


@unittest.skipIf(sys.platform == "win32", "runs a bash script")
class RetryTest(unittest.TestCase):
    def setUp(self):
        self._temp = tempfile.TemporaryDirectory()
        self.counter = Path(self._temp.name) / "count"

    def tearDown(self):
        self._temp.cleanup()

    def retry(self, attempts, failures, status=7):
        command = ["bash", "-c", FLAKY, "flaky", str(self.counter), str(failures), str(status)]
        return subprocess.run(
            ["bash", str(SCRIPT), str(attempts), "0", *command],
            capture_output=True,
            text=True,
            check=False,
        )

    def runs(self):
        return int(self.counter.read_text())

    def test_stops_at_the_first_success(self):
        result = self.retry(attempts=3, failures=0)
        self.assertEqual(result.returncode, 0)
        self.assertEqual(self.runs(), 1)
        self.assertEqual(result.stderr, "")

    def test_retries_until_the_command_succeeds(self):
        result = self.retry(attempts=3, failures=2)
        self.assertEqual(result.returncode, 0)
        self.assertEqual(self.runs(), 3)
        self.assertIn("attempt 2 of 3", result.stderr)

    def test_gives_up_with_the_last_status(self):
        result = self.retry(attempts=3, failures=5, status=9)
        self.assertEqual(result.returncode, 9)
        self.assertEqual(self.runs(), 3)
        self.assertIn("in all 3 attempts", result.stderr)

    def test_rejects_bad_arguments(self):
        for arguments in (["3", "0"], ["x", "0", "true"], ["3", "", "true"], ["0", "0", "true"]):
            result = subprocess.run(
                ["bash", str(SCRIPT), *arguments], capture_output=True, text=True, check=False
            )
            self.assertEqual(result.returncode, 2, arguments)


if __name__ == "__main__":
    unittest.main()
