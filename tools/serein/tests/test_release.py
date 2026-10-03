import hashlib
import json
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

import release

INFO = {
    "repo": "owner/SereinGram",
    "tag": "nightly",
    "channel": "nightly",
    "version": "6.2.4",
    "commit": "a" * 40,
    "date": "2026-10-02",
    "previous": "b" * 40,
    "since": "the previous nightly",
}


def git(root, *args, message=None):
    command = ["git", "-c", "user.name=T", "-c", "user.email=t@e", *args]
    if message is not None:
        command += ["-m", message]
    return subprocess.run(
        command, cwd=root, check=True, capture_output=True, text=True
    ).stdout.strip()


class AssetsTest(unittest.TestCase):
    def test_policy_follows_the_naming_rule(self):
        assets = release.load_assets()
        names = [asset["name"] for asset in assets]
        self.assertIn("SereinGram-linux-x86_64.AppImage", names)
        for arch in ("universal", "arm64", "x86_64"):
            self.assertIn(f"SereinGram-macos-{arch}.dmg", names)
        self.assertEqual(len(names), len(set(names)))

    def test_rejects_names_outside_the_rule(self):
        for name in (
            "SereinGram-win-x64.exe",
            "Serein-linux-x86_64.deb",
            "SereinGram-linux-amd64.deb",
            "SereinGram-linux-x86_64.tar.gz",
        ):
            self.assertIsNone(release.NAME.match(name), name)

    def test_rejects_disagreeing_metadata(self):
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / "assets.json"
            path.write_text(
                json.dumps(
                    {
                        "schema_version": 1,
                        "assets": [
                            {
                                "name": "SereinGram-linux-x86_64.deb",
                                "os": "linux",
                                "arch": "arm64",
                                "kind": "deb",
                            }
                        ],
                    }
                )
            )
            with self.assertRaises(release.ReleaseError):
                release.load_assets(path)


DEB_ASSET = {
    "name": "SereinGram-linux-x86_64.deb",
    "os": "linux",
    "arch": "x86_64",
    "kind": "deb",
}


class CompatibilityTest(unittest.TestCase):
    def test_allows_added_assets(self):
        extra = dict(DEB_ASSET, name="SereinGram-linux-arm64.deb", arch="arm64")
        self.assertEqual(release.compatibility([DEB_ASSET], [DEB_ASSET, extra]), [])

    def test_rejects_removed_or_changed_assets(self):
        self.assertTrue(release.compatibility([DEB_ASSET], [])[0].endswith("renamed"))
        changed = dict(DEB_ASSET, kind="rpm")
        self.assertIn("cannot change", release.compatibility([DEB_ASSET], [changed])[0])

    def test_compares_with_a_git_baseline(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            git(root, "init", "-q")
            policy = root / release.POLICY_PATH
            policy.parent.mkdir(parents=True)
            policy.write_text(json.dumps({"schema_version": 1, "assets": [DEB_ASSET]}))
            git(root, "add", ".")
            git(root, "commit", "-q", message="chore: assets")
            self.assertEqual(release.check_compatibility(root, "HEAD", [DEB_ASSET]), 0)
            self.assertEqual(release.check_compatibility(root, "HEAD", []), 1)
            self.assertEqual(release.check_compatibility(root, "0" * 40, []), 0)


class FilesTest(unittest.TestCase):
    def setUp(self):
        self._temp = tempfile.TemporaryDirectory()
        self.dir = Path(self._temp.name)
        self.assets = [
            {"name": "SereinGram-linux-x86_64.deb", "os": "linux", "arch": "x86_64", "kind": "deb"},
            {
                "name": "SereinGram-macos-universal.dmg",
                "os": "macos",
                "arch": "universal",
                "kind": "disk-image",
            },
        ]
        for index, asset in enumerate(self.assets):
            (self.dir / asset["name"]).write_bytes(bytes([index]) * 10)

    def tearDown(self):
        self._temp.cleanup()

    def test_verify_reports_missing_and_unexpected(self):
        self.assertEqual(release.verify(self.dir, self.assets), ([], []))
        (self.dir / "SereinGram-linux-x86_64.deb").unlink()
        (self.dir / "extra.txt").write_text("x")
        self.assertEqual(
            release.verify(self.dir, self.assets), (["SereinGram-linux-x86_64.deb"], ["extra.txt"])
        )

    def test_checksums_use_the_sha256sum_format(self):
        text = release.checksums(self.dir, list(reversed(self.assets)))
        lines = text.splitlines()
        self.assertEqual(len(lines), 2)
        digest = hashlib.sha256(bytes([0]) * 10).hexdigest()
        self.assertEqual(lines[0], f"{digest}  SereinGram-linux-x86_64.deb")
        self.assertTrue(lines[1].endswith("  SereinGram-macos-universal.dmg"))
        self.assertTrue(text.endswith("\n"))

    def test_manifest_lists_hashes_and_urls(self):
        data = json.loads(release.manifest(self.dir, self.assets, INFO))
        self.assertEqual(data["schema_version"], 1)
        self.assertEqual(data["channel"], "nightly")
        first = data["assets"][0]
        self.assertEqual(first["size"], 10)
        self.assertEqual(first["sha256"], hashlib.sha256(bytes([0]) * 10).hexdigest())
        self.assertEqual(
            first["url"],
            "https://github.com/owner/SereinGram/releases/"
            "download/nightly/SereinGram-linux-x86_64.deb",
        )


class UploadedTest(unittest.TestCase):
    def setUp(self):
        self._temp = tempfile.TemporaryDirectory()
        self.dir = Path(self._temp.name) / "dist"
        self.dir.mkdir()
        (self.dir / "SHA256SUMS").write_bytes(b"x" * 10)
        (self.dir / "SereinGram-macos-arm64.dmg").write_bytes(b"x" * 20)

    def tearDown(self):
        self._temp.cleanup()

    @staticmethod
    def release(*assets):
        return {
            "assets": [{"name": name, "size": size, "state": state} for name, size, state in assets]
        }

    def test_accepts_the_same_files(self):
        release_data = self.release(
            ("SHA256SUMS", 10, "uploaded"), ("SereinGram-macos-arm64.dmg", 20, "uploaded")
        )
        self.assertEqual(release.uploaded(self.dir, release_data), [])

    def test_reports_every_difference(self):
        release_data = self.release(
            ("SHA256SUMS", 9, "uploaded"),
            ("SereinGram-macos-arm64.dmg", 20, "starter"),
            ("old.txt", 1, "uploaded"),
        )
        self.assertEqual(
            release.uploaded(self.dir, release_data),
            [
                "SereinGram-macos-arm64.dmg: the upload did not finish",
                "SereinGram-macos-arm64.dmg: missing from the release",
                "old.txt: not part of this build",
                "SHA256SUMS: 9 bytes uploaded, 10 built",
            ],
        )

    def test_main_reads_the_gh_output(self):
        output = Path(self._temp.name) / "release.json"
        output.write_text(json.dumps(self.release(("SHA256SUMS", 10, "uploaded"))))
        self.assertEqual(release.main(["uploaded", str(self.dir), str(output)]), 1)
        (self.dir / "SereinGram-macos-arm64.dmg").unlink()
        self.assertEqual(release.main(["uploaded", str(self.dir), str(output)]), 0)
        output.write_text("not json")
        self.assertEqual(release.main(["uploaded", str(self.dir), str(output)]), 1)


class NotesTest(unittest.TestCase):
    def setUp(self):
        self._temp = tempfile.TemporaryDirectory()
        self.root = Path(self._temp.name)
        git(self.root, "init", "-q", "-b", "develop")
        self.base = self.commit("feat: start")

    def tearDown(self):
        self._temp.cleanup()

    def commit(self, message):
        git(self.root, "commit", "-q", "--allow-empty", message=message)
        return git(self.root, "rev-parse", "HEAD")

    def build(self):
        self.commit("feat(privacy): show the contact relationship")
        self.commit("fix(windows): read credentials quietly")
        self.commit("perf(filters): compile patterns once")
        self.commit("feat(menu)!: drop the old menu format")
        self.commit(
            "chore(upstream): merge Telegram Desktop dev\n\n"
            "- Upstream baseline: 111111111111 -> 222222222222."
        )
        self.commit("docs(readme): list the packages")
        self.commit("Fix the build.")
        head = self.commit("ci(linux): keep one compiler cache")
        return release.first_parent_log(self.root, self.base, head, 50), head

    def test_groups_changes_by_type(self):
        commits, head = self.build()
        text = release.notes(
            commits, release.load_assets(), dict(INFO, commit=head, previous=self.base)
        )
        self.assertIn("## Changes since the previous nightly", text)
        self.assertIn("8 commits", text)
        self.assertIn("### Breaking changes\n\n- **menu:** Drop the old menu", text)
        self.assertIn("### Features\n\n- **privacy:** Show the contact", text)
        self.assertIn("### Fixes\n\n- **windows:** Read credentials", text)
        self.assertIn("### Performance\n\n- **filters:** Compile", text)
        self.assertIn("- Merged Telegram Desktop `dev` up to [`2222222222`]", text)
        self.assertIn("<summary>3 other changes", text)
        self.assertIn("- Fix the build. ([`", text)
        self.assertNotIn("start", text.split("## Downloads")[0])
        self.assertLess(text.index("## Changes"), text.index("## Downloads"))
        self.assertIn(
            "| Linux | x86_64 | AppImage | [SereinGram-linux-"
            "x86_64.AppImage](https://github.com/owner/SereinGram/"
            "releases/download/nightly/",
            text,
        )
        self.assertIn("sha256sum --ignore-missing -c SHA256SUMS", text)

    def test_notes_explain_the_first_launch_of_unsigned_builds(self):
        text = release.notes([], release.load_assets(), dict(INFO, previous=""))
        verify = text.split("## Verify")[1]
        self.assertIn("Open Anyway in System Settings > Privacy & Security", verify)
        self.assertIn("More info and Run anyway", verify)

    def test_notes_announce_test_credentials_first(self):
        text = release.notes(
            [], release.load_assets(), dict(INFO, previous="", test_credentials=True)
        )
        notice = "\n".join(release.credentials_notice())
        self.assertTrue(text.startswith(notice))
        self.assertIn("please be patient", notice)
        self.assertLess(text.index("please be patient"), text.index("**SereinGram"))
        plain = release.notes([], release.load_assets(), dict(INFO, previous=""))
        self.assertNotIn("test credentials", plain)
        self.assertTrue(plain.startswith("**SereinGram Nightly**"))

    def test_first_build_takes_the_latest_commits(self):
        self.build()
        commits = release.first_parent_log(self.root, "", "HEAD", 3)
        self.assertEqual(len(commits), 3)
        text = release.notes(commits, release.load_assets(), dict(INFO, previous=""))
        self.assertIn("First build of this channel, latest 3 commits.", text)

    def test_main_writes_notes_and_reads_the_version(self):
        _, head = self.build()
        version = self.root / "Telegram" / "build" / "version"
        version.parent.mkdir(parents=True)
        version.write_text("AppVersion 6002004\nAppVersionStr 6.2.4\n")
        output = self.root / "notes.md"
        self.assertEqual(release.read_version(self.root), "6.2.4")
        code = release.main(
            [
                "notes",
                "--repo",
                "o/r",
                "--tag",
                "nightly",
                "--channel",
                "nightly",
                "--commit",
                head,
                "--date",
                "2026-10-02",
                "--version",
                "6.2.4",
                "--previous",
                self.base,
                "--root",
                str(self.root),
                "-o",
                str(output),
            ]
        )
        self.assertEqual(code, 0)
        self.assertIn("**SereinGram Nightly**", output.read_text())
        self.assertNotIn("please be patient", output.read_text())

    def test_main_passes_the_test_credentials_flag(self):
        _, head = self.build()
        output = self.root / "notes.md"
        code = release.main(
            [
                "notes",
                "--repo",
                "o/r",
                "--tag",
                "nightly",
                "--channel",
                "nightly",
                "--commit",
                head,
                "--date",
                "2026-10-03",
                "--version",
                "6.2.4",
                "--previous",
                self.base,
                "--root",
                str(self.root),
                "--test-credentials",
                "-o",
                str(output),
            ]
        )
        self.assertEqual(code, 0)
        notice = "\n".join(release.credentials_notice())
        self.assertTrue(output.read_text().startswith(notice))


if __name__ == "__main__":
    unittest.main()
