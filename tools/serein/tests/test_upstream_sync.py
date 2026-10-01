import json
import shutil
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

import upstream_sync  # noqa: E402


def git(root, *args):
    return subprocess.run(
        ["git", "-c", "user.name=t", "-c", "user.email=t@t", *args],
        cwd=root, capture_output=True, check=True, text=True).stdout.strip()


def write(root, path, text):
    target = Path(root) / path
    target.parent.mkdir(parents=True, exist_ok=True)
    target.write_text(text)


class UpstreamSyncTest(unittest.TestCase):
    def setUp(self):
        self._temp = tempfile.TemporaryDirectory()
        temp = Path(self._temp.name)
        self.upstream = temp / "upstream"
        self.fork = temp / "fork"
        self.upstream.mkdir()
        git(self.upstream, "init", "-q", "-b", "dev")
        write(self.upstream, "Telegram/SourceFiles/a.cpp", "int a;\n")
        write(self.upstream, "Telegram/SourceFiles/b.cpp", "int b;\n")
        git(self.upstream, "add", ".")
        git(self.upstream, "commit", "-q", "-m", "base")
        self.base = git(self.upstream, "rev-parse", "HEAD")
        git(temp, "clone", "-q", str(self.upstream), str(self.fork))
        git(self.fork, "config", "user.name", "t")
        git(self.fork, "config", "user.email", "t@t")
        self.policy = self.fork / "tools/policy/upstream.json"
        self.owned = self.fork / "tools/policy/owned.json"
        write(self.fork, "tools/policy/owned.json", json.dumps({
            "schema_version": 1, "max_lines": 1000,
            "owned": ["Telegram/SourceFiles/serein/", "tools/"],
            "extensions": [".cpp", ".json"], "filenames": [],
        }))
        write(self.fork, "tools/policy/upstream.json", json.dumps({
            "schema_version": 1,
            "upstream": f"{self.upstream}#dev",
            "base": self.base,
            "source_root": "Telegram/SourceFiles/",
            "owned_extra": [],
            "own_include_prefixes": ["serein/"],
            "hook_include_prefixes": ["serein/hooks/"],
            "budget": dict.fromkeys(upstream_sync.upstream_budget.BUDGET_KEYS, 100),
        }))
        write(self.fork, "Telegram/SourceFiles/serein/x.cpp", "int x;\n")
        write(self.fork, "Telegram/SourceFiles/a.cpp", "int a; // hook\n")
        git(self.fork, "add", ".")
        git(self.fork, "commit", "-q", "-m", "serein")

    def tearDown(self):
        self._temp.cleanup()

    def upstream_commit(self, path, text, tag):
        write(self.upstream, path, text)
        git(self.upstream, "commit", "-q", "-am", tag)
        git(self.upstream, "tag", tag)
        return git(self.upstream, "rev-parse", "HEAD")

    def sync(self, ref):
        return upstream_sync.sync(self.fork, ref, self.policy, self.owned)

    def test_clean_merge_moves_the_baseline(self):
        head = self.upstream_commit("Telegram/SourceFiles/b.cpp", "int b2;\n", "v2")
        conflicts, metrics = self.sync("v2")
        self.assertIsNone(conflicts)
        self.assertEqual(metrics["source_files"], 1)
        self.assertEqual(json.loads(self.policy.read_text())["base"], head)
        self.assertEqual(git(self.fork, "branch", "--show-current"), "sync/v2")
        message = git(self.fork, "log", "-1", "--format=%B")
        self.assertTrue(message.startswith("chore(upstream): merge Telegram Desktop v2"))
        self.assertEqual(git(self.fork, "rev-parse", "HEAD^2"), head)
        self.assertEqual((self.fork / "Telegram/SourceFiles/b.cpp").read_text(), "int b2;\n")

    def test_conflicts_are_grouped_by_owner(self):
        self.upstream_commit("Telegram/SourceFiles/a.cpp", "int a2;\n", "v3")
        conflicts, metrics = self.sync("v3")
        self.assertIsNone(metrics)
        self.assertEqual(conflicts, {
            "serein": [], "hooks": ["Telegram/SourceFiles/a.cpp"], "upstream": [],
        })
        self.assertEqual(json.loads(self.policy.read_text())["base"], self.base)

    def test_submodule_pointer_conflicts_take_the_newer_commit(self):
        self.check_submodule_conflict(initialized=True)

    def test_uninitialized_submodules_are_fetched_before_merging(self):
        self.check_submodule_conflict(initialized=False)

    def check_submodule_conflict(self, initialized):
        lib = Path(self._temp.name) / "lib"
        lib.mkdir()
        git(lib, "init", "-q", "-b", "main")
        commits = []
        for name in ("c1", "c2", "c3"):
            write(lib, "f.txt", name + "\n")
            git(lib, "add", ".")
            git(lib, "commit", "-q", "-m", name)
            commits.append(git(lib, "rev-parse", "HEAD"))
        allow = ("-c", "protocol.file.allow=always")
        git(self.upstream, *allow, "submodule", "add", "-q", str(lib), "cmake")
        git(self.upstream / "cmake", "checkout", "-q", commits[0])
        git(self.upstream, "add", "cmake")
        git(self.upstream, "commit", "-q", "-m", "submodule")
        git(self.fork, "pull", "-q", "--no-rebase", "--no-edit", "origin", "dev")
        git(self.fork, *allow, "submodule", "update", "-q", "--init")
        git(self.fork / "cmake", "checkout", "-q", commits[1])
        git(self.fork, "commit", "-q", "-am", "bump to c2")
        git(self.upstream / "cmake", "checkout", "-q", commits[2])
        git(self.upstream, "commit", "-q", "-am", "bump to c3")
        git(self.upstream, "tag", "v4")
        if not initialized:
            module_git = git(self.fork / "cmake", "rev-parse", "--absolute-git-dir")
            git(self.fork, "submodule", "deinit", "-q", "-f", "cmake")
            shutil.rmtree(module_git)
        conflicts, metrics = self.sync("v4")
        self.assertIsNone(conflicts)
        self.assertIsNotNone(metrics)
        self.assertEqual(git(self.fork, "rev-parse", "HEAD:cmake"), commits[2])

    def test_dirty_worktree_is_refused(self):
        write(self.fork, "Telegram/SourceFiles/b.cpp", "dirty\n")
        with self.assertRaisesRegex(upstream_sync.SyncError, "uncommitted"):
            self.sync("dev")

    def test_ref_must_contain_the_baseline(self):
        git(self.upstream, "checkout", "-q", "--orphan", "other")
        write(self.upstream, "c.txt", "c\n")
        git(self.upstream, "add", "c.txt")
        git(self.upstream, "commit", "-q", "-m", "unrelated")
        with self.assertRaisesRegex(upstream_sync.SyncError, "baseline"):
            self.sync("other")

    def test_branch_names_are_sanitized(self):
        self.assertEqual(upstream_sync.branch_name("refs/tags/v6.3 beta"),
                         "sync/refs-tags-v6.3-beta")


if __name__ == "__main__":
    unittest.main()
