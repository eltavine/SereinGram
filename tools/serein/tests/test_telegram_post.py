import json
import os
import stat
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path
from unittest import mock

import git_env

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

import release
import telegram_post

TOKEN = "123:secret"
FAKE_CURL = """\
#!/usr/bin/env python3
import json
import os
import sys

args = sys.argv[1:]
with open(os.environ["FAKE_CURL_LOG"], "a") as log:
    log.write(json.dumps(args) + "\\n")
method = args[-1].rsplit("/", 1)[-1]
with open(os.environ["FAKE_CURL_LOG"]) as log:
    count = sum(json.loads(line)[-1].endswith("/" + method) for line in log)
plan = json.loads(os.environ.get("FAKE_CURL_PLAN", "{}")).get(method, [])
default = {"ok": True, "result": {"message_id": 7}}
print(json.dumps(plan[min(count, len(plan)) - 1] if plan else default))
"""


def setUpModule():
    git_env.disable_background_maintenance()


def git(root, *args):
    command = ["git", "-c", "user.name=Ada Lovelace", "-c", "user.email=a@e", *args]
    subprocess.run(command, cwd=root, check=True, capture_output=True)


class Harness(unittest.TestCase):
    def setUp(self):
        self._temp = tempfile.TemporaryDirectory()
        self.dir = Path(self._temp.name)
        bin_dir = self.dir / "bin"
        bin_dir.mkdir()
        curl = bin_dir / "curl"
        curl.write_text(FAKE_CURL)
        curl.chmod(curl.stat().st_mode | stat.S_IXUSR)
        self.log = self.dir / "curl.log"
        self.environment = {
            "PATH": f"{bin_dir}{os.pathsep}{os.environ['PATH']}",
            "FAKE_CURL_LOG": str(self.log),
        }
        self.dist = self.dir / "dist"
        self.dist.mkdir()
        self.assets = release.load_assets()
        for asset in self.assets:
            (self.dist / asset["name"]).write_bytes(b"x")

    def tearDown(self):
        self._temp.cleanup()

    def calls(self):
        return [json.loads(line) for line in self.log.read_text().splitlines()]

    @staticmethod
    def method(args):
        return args[-1].rsplit("/", 1)[-1]

    @staticmethod
    def fields(args):
        pairs = (args[index + 1] for index, arg in enumerate(args) if arg == "--form-string")
        return dict(pair.split("=", 1) for pair in pairs)

    @staticmethod
    def uploads(args):
        return [args[index + 1] for index, arg in enumerate(args) if arg == "--form"]


class HeaderTest(unittest.TestCase):
    def test_escapes_the_commit_and_links_it(self):
        text = telegram_post.header(
            "SereinGram Nightly",
            'fix(ui): keep <b> & "quotes"',
            "Ada <ada@e>",
            "0123456789abcdef",
            "https://github.com/o/r/commit/0123456789abcdef",
            "42",
            "https://github.com/o/r/actions/runs/9?a=1&b=2",
        )
        self.assertIn("<pre>fix(ui): keep &lt;b&gt; &amp; &quot;quotes&quot;</pre>", text)
        self.assertIn("Author: <b>Ada &lt;ada@e&gt;</b>", text)
        self.assertIn(
            'Commit: <a href="https://github.com/o/r/commit/0123456789abcdef">01234567</a>', text
        )
        self.assertIn(
            'Action: <a href="https://github.com/o/r/actions/runs/9?a=1&amp;b=2">#42</a>', text
        )

    def test_shortens_a_long_subject(self):
        text = telegram_post.header("T", "x" * 600, "A", "0" * 40, "u", "1", "v")
        subject = text.split("<pre>")[1].split("</pre>")[0]
        self.assertEqual(len(subject), telegram_post.SUBJECT)
        self.assertTrue(subject.endswith("\u2026"))


class AlbumsTest(Harness):
    def test_groups_the_binaries_by_system(self):
        groups = telegram_post.albums(self.dist, self.assets)
        names = [[path.name for path in group] for group in groups]
        self.assertEqual([len(group) for group in names], [4, 3, 7, 5])
        self.assertTrue(all(name.startswith("SereinGram-windows-") for name in names[0]))
        self.assertTrue(all("-linux-x86_64" in name for name in names[2]))
        self.assertTrue(all("-linux-arm64" in name for name in names[3]))
        self.assertFalse(any(name.endswith(".zsync") for group in names for name in group))

    def test_refuses_a_missing_binary(self):
        (self.dist / "SereinGram-macos-arm64.dmg").unlink()
        with self.assertRaises(telegram_post.TelegramError):
            telegram_post.albums(self.dist, self.assets)


class CallTest(Harness):
    def call(self, plan, **kwargs):
        waits = []
        environment = dict(self.environment, FAKE_CURL_PLAN=json.dumps(plan))
        with mock.patch.dict(os.environ, environment):
            result = telegram_post.call(
                "http://bot", TOKEN, "sendMessage", {"text": "@hi"}, sleep=waits.append, **kwargs
            )
        return result, waits

    def test_waits_out_flood_control(self):
        plan = {
            "sendMessage": [
                {"ok": False, "error_code": 429, "parameters": {"retry_after": 3}},
                {"ok": True, "result": {"message_id": 5}},
            ]
        }
        result, waits = self.call(plan)
        self.assertEqual(result, {"message_id": 5})
        self.assertEqual(waits, [3])
        args = self.calls()[0]
        self.assertEqual(self.fields(args), {"text": "@hi"})
        self.assertEqual(args[-1], f"http://bot/bot{TOKEN}/sendMessage")

    def test_stops_at_a_client_error_without_leaking_the_token(self):
        plan = {"sendMessage": [{"ok": False, "error_code": 400, "description": f"bad {TOKEN}"}]}
        with self.assertRaises(telegram_post.TelegramError) as raised:
            self.call(plan)
        self.assertEqual(len(self.calls()), 1)
        self.assertNotIn(TOKEN, str(raised.exception))

    def test_retries_a_server_that_does_not_answer(self):
        plan = {"sendMessage": [{}, {}, {"ok": True, "result": True}]}
        result, waits = self.call(plan, delays=(1, 2, 3))
        self.assertTrue(result)
        self.assertEqual(waits, [1, 2])


class MainTest(Harness):
    def setUp(self):
        super().setUp()
        self.root = self.dir / "repo"
        self.root.mkdir()
        git(self.root, "init", "-q")
        git(self.root, "commit", "-q", "--allow-empty", "-m", "feat(app): ship <it>\n\nBody.")
        self.commit = subprocess.run(
            ["git", "rev-parse", "HEAD"], cwd=self.root, check=True, capture_output=True, text=True
        ).stdout.strip()

    def run_main(self, token=TOKEN, server="http://bot/", plan=None):
        environment = dict(
            self.environment, TELEGRAM_BOT_TOKEN=token, FAKE_CURL_PLAN=json.dumps(plan or {})
        )
        if server:
            environment["TELEGRAM_BOT_API"] = server
        arguments = [
            str(self.dist),
            "--chat",
            "-1001",
            "--title",
            "SereinGram Nightly",
            "--commit",
            self.commit,
            "--commit-url",
            f"https://github.com/o/r/commit/{self.commit}",
            "--run-number",
            "12",
            "--run-url",
            "https://github.com/o/r/actions/runs/34",
            "--release-url",
            "https://github.com/o/r/releases/tag/nightly",
            "--root",
            str(self.root),
        ]
        with mock.patch.dict(os.environ, environment):
            if not server:
                os.environ.pop("TELEGRAM_BOT_API", None)
            return telegram_post.main(arguments)

    def assert_plain_text(self, args):
        self.assertEqual(args[-1], f"{telegram_post.PUBLIC_API}/bot{TOKEN}/sendMessage")
        fields = self.fields(args)
        self.assertNotIn("parse_mode", fields)
        self.assertEqual(
            fields["text"].splitlines(),
            [
                "SereinGram Nightly",
                "",
                "feat(app): ship <it>",
                "Author: Ada Lovelace",
                f"Commit: https://github.com/o/r/commit/{self.commit}",
                "Action: https://github.com/o/r/actions/runs/34",
                "Downloads: https://github.com/o/r/releases/tag/nightly",
            ],
        )
        self.assertEqual(self.uploads(args), [])

    def test_posts_the_commit_then_every_album(self):
        self.assertEqual(self.run_main(), 0)
        calls = self.calls()
        methods = [self.method(args) for args in calls]
        self.assertEqual(methods, ["getMe", "sendMessage"] + ["sendMediaGroup"] * 4)
        message = self.fields(calls[1])
        self.assertEqual(message["chat_id"], "-1001")
        self.assertEqual(message["parse_mode"], "HTML")
        self.assertIn("<pre>feat(app): ship &lt;it&gt;</pre>", message["text"])
        self.assertIn("Author: <b>Ada Lovelace</b>", message["text"])
        self.assertIn("/actions/runs/34", message["text"])
        posted = []
        for args in calls[2:]:
            fields = self.fields(args)
            self.assertEqual(json.loads(fields["reply_parameters"])["message_id"], 7)
            media = json.loads(fields["media"])
            uploads = self.uploads(args)
            self.assertEqual(len(media), len(uploads))
            posted += [Path(upload.split("=@", 1)[1]).name for upload in uploads]
        expected = [a["name"] for a in self.assets if a["kind"] not in release.UPDATE_DATA]
        self.assertEqual(sorted(posted), sorted(expected))

    def test_needs_the_bot_token(self):
        self.assertEqual(self.run_main(token=""), 1)
        self.assertFalse(self.log.exists())

    def test_sends_plain_text_without_a_local_server(self):
        self.assertEqual(self.run_main(server=""), 0)
        calls = self.calls()
        self.assertEqual(len(calls), 1)
        self.assert_plain_text(calls[0])

    def test_sends_plain_text_when_the_files_fail(self):
        plan = {"sendMediaGroup": [{"ok": False, "error_code": 400, "description": "too big"}]}
        self.assertEqual(self.run_main(plan=plan), 0)
        calls = self.calls()
        methods = [self.method(args) for args in calls]
        self.assertEqual(methods, ["getMe", "sendMessage", "sendMediaGroup", "sendMessage"])
        self.assertTrue(calls[0][-1].startswith("http://bot/"))
        self.assert_plain_text(calls[-1])

    def test_fails_when_even_the_text_cannot_be_sent(self):
        plan = {"sendMessage": [{"ok": False, "error_code": 403, "description": "not a member"}]}
        self.assertEqual(self.run_main(server="", plan=plan), 1)


if __name__ == "__main__":
    unittest.main()
