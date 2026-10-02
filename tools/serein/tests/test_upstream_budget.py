import contextlib
import io
import json
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

import upstream_budget


def git(root, *args):
    return subprocess.run(
        ["git", "-c", "user.name=t", "-c", "user.email=t@t", *args],
        cwd=root,
        capture_output=True,
        check=True,
        text=True,
    ).stdout.strip()


class UpstreamBudgetTest(unittest.TestCase):
    def setUp(self):
        self._temp = tempfile.TemporaryDirectory()
        self.root = Path(self._temp.name)
        git(self.root, "init", "-q")
        self.write("Telegram/SourceFiles/a.cpp", "int a;\n")
        self.write("Telegram/SourceFiles/b.cpp", "int b;\n")
        self.write("lib/x.txt", "x\n")
        git(self.root, "add", ".")
        git(self.root, "commit", "-q", "-m", "base")
        self.base = git(self.root, "rev-parse", "HEAD")
        self._policies = tempfile.TemporaryDirectory()
        policies = Path(self._policies.name)
        self.owned = policies / "owned.json"
        self.owned.write_text(
            json.dumps(
                {
                    "schema_version": 1,
                    "max_lines": 1000,
                    "owned": ["Telegram/SourceFiles/serein/"],
                    "extensions": [".cpp"],
                    "filenames": [],
                }
            ),
            encoding="utf-8",
        )
        self.policy = policies / "upstream.json"

    def tearDown(self):
        self._policies.cleanup()
        self._temp.cleanup()

    def write(self, name, text):
        path = self.root / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(text, encoding="utf-8")

    def write_policy(self, overrides=(), **budget):
        overrides = list(overrides)
        values = dict.fromkeys(upstream_budget.BUDGET_KEYS, 100)
        values.update(budget)
        self.policy.write_text(
            json.dumps(
                {
                    "schema_version": 1,
                    "upstream": "test",
                    "base": self.base,
                    "source_root": "Telegram/SourceFiles/",
                    "owned_extra": ["docs/"],
                    "own_include_prefixes": ["serein/"],
                    "hook_include_prefixes": ["serein/hooks/"],
                    "submodule_overrides": overrides,
                    "budget": values,
                }
            ),
            encoding="utf-8",
        )

    def run_budget(self, *extra):
        output = io.StringIO()
        with contextlib.redirect_stdout(output), contextlib.redirect_stderr(output):
            code = upstream_budget.main(
                [
                    "--root",
                    str(self.root),
                    "--policy",
                    str(self.policy),
                    "--owned-policy",
                    str(self.owned),
                    *extra,
                ]
            )
        return code, output.getvalue()

    def change_tree(self):
        self.write("Telegram/SourceFiles/a.cpp", 'int a;\n#include "serein/features/x.h"\nint c;\n')
        self.write("Telegram/SourceFiles/b.cpp", 'int b;\n#include "serein/hooks/ghost.h"\n')
        self.write("Telegram/SourceFiles/serein/own.cpp", "int own;\n" * 50)
        self.write("docs/notes.md", "notes\n")
        self.write("lib/x.txt", "x\ny\n")
        git(self.root, "add", ".")
        git(self.root, "commit", "-q", "-m", "change")

    def test_measures_only_upstream_files(self):
        self.change_tree()
        self.write_policy()
        policy = upstream_budget.load_policy(self.policy)
        metrics, offenders = upstream_budget.measure(
            self.root, policy, ["Telegram/SourceFiles/serein/"]
        )
        self.assertEqual(
            metrics,
            {
                "all_files": 3,
                "all_added_lines": 4,
                "source_files": 2,
                "source_added_lines": 3,
                "direct_include_files": 1,
            },
        )
        self.assertEqual(offenders, ["Telegram/SourceFiles/a.cpp"])

    def test_passes_within_budget(self):
        self.change_tree()
        self.write_policy(direct_include_files=1)
        code, output = self.run_budget("--list")
        self.assertEqual(code, 0, output)
        self.assertIn("direct include: Telegram/SourceFiles/a.cpp", output)

    def test_fails_over_budget(self):
        self.change_tree()
        self.write_policy(direct_include_files=0, source_files=1)
        code, output = self.run_budget()
        self.assertEqual(code, 1)
        self.assertIn("source_files, direct_include_files", output)

    def link(self, path, sha):
        git(self.root, "update-index", "--add", "--cacheinfo", f"160000,{sha},{path}")

    def test_reports_submodule_behind_upstream(self):
        upstream = "1" * 40
        self.link("deps/lib", upstream)
        git(self.root, "commit", "-q", "-m", "add submodule")
        self.base = git(self.root, "rev-parse", "HEAD")
        self.link("deps/lib", "2" * 40)
        self.link("deps/own", "3" * 40)
        self.write_policy()
        code, output = self.run_budget()
        self.assertEqual(code, 1, output)
        self.assertIn("Submodule deps/lib is staged at 2222222222", output)
        self.assertIn("upstream base has 1111111111", output)
        self.assertNotIn("deps/own", output)
        self.write_policy(overrides=["deps/lib"])
        code, output = self.run_budget()
        self.assertEqual(code, 0, output)

    def test_rejects_short_base(self):
        self.write_policy()
        policy = json.loads(self.policy.read_text(encoding="utf-8"))
        policy["base"] = self.base[:10]
        self.policy.write_text(json.dumps(policy), encoding="utf-8")
        code, output = self.run_budget()
        self.assertEqual(code, 2)
        self.assertIn("40-character", output)


if __name__ == "__main__":
    unittest.main()
