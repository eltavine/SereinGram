import contextlib
import io
import json
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

import check_file_size


class CheckFileSizeTest(unittest.TestCase):
    def setUp(self):
        self._temp = tempfile.TemporaryDirectory()
        self.root = Path(self._temp.name)
        subprocess.run(["git", "init", "-q"], cwd=self.root, check=True)
        self.policy = self.root / "policy.json"
        self.write_policy(max_lines=3)

    def tearDown(self):
        self._temp.cleanup()

    def write_policy(self, **overrides):
        policy = {
            "schema_version": 1,
            "max_lines": 3,
            "owned": ["own/", "tools/*.py"],
            "extensions": [".cpp", ".py"],
            "filenames": ["CMakeLists.txt"],
        }
        policy.update(overrides)
        self.policy.write_text(json.dumps(policy), encoding="utf-8")

    def write(self, name, lines):
        path = self.root / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text("".join(f"line {i}\n" for i in range(lines)), encoding="utf-8")

    def run_check(self):
        output = io.StringIO()
        with contextlib.redirect_stdout(output), contextlib.redirect_stderr(output):
            code = check_file_size.main(["--root", str(self.root), "--policy", str(self.policy)])
        return code, output.getvalue()

    def test_passes_at_limit(self):
        self.write("own/a.cpp", 3)
        code, output = self.run_check()
        self.assertEqual(code, 0, output)

    def test_fails_over_limit_in_owned_directory(self):
        self.write("own/nested/b.cpp", 4)
        code, output = self.run_check()
        self.assertEqual(code, 1)
        self.assertIn("own/nested/b.cpp: 4 lines", output)

    def test_ignores_upstream_files(self):
        self.write("upstream/big.cpp", 50)
        code, output = self.run_check()
        self.assertEqual(code, 0, output)

    def test_ignores_non_source_extensions(self):
        self.write("own/notes.md", 50)
        code, output = self.run_check()
        self.assertEqual(code, 0, output)

    def test_checks_named_files_and_globs(self):
        self.write("own/CMakeLists.txt", 4)
        self.write("tools/gen.py", 4)
        code, output = self.run_check()
        self.assertEqual(code, 1)
        self.assertIn("own/CMakeLists.txt", output)
        self.assertIn("tools/gen.py", output)

    def test_ignores_generated_files(self):
        self.write_policy(generated=["own/gen/", "own/*.lock.cpp"])
        self.write("own/gen/a.cpp", 50)
        self.write("own/deps.lock.cpp", 50)
        code, output = self.run_check()
        self.assertEqual(code, 0, output)
        self.write("own/b.cpp", 4)
        code, output = self.run_check()
        self.assertEqual(code, 1)
        self.assertIn("own/b.cpp", output)
        self.assertNotIn("own/gen/a.cpp", output)

    def test_respects_gitignore(self):
        (self.root / ".gitignore").write_text("own/build/\n", encoding="utf-8")
        self.write("own/build/generated.cpp", 50)
        code, output = self.run_check()
        self.assertEqual(code, 0, output)

    def test_rejects_malformed_policy(self):
        self.write_policy(max_lines=0)
        code, output = self.run_check()
        self.assertEqual(code, 2)
        self.assertIn("max_lines", output)

    def test_rejects_unknown_policy_keys(self):
        self.write_policy(extra=True)
        code, _ = self.run_check()
        self.assertEqual(code, 2)


if __name__ == "__main__":
    unittest.main()
