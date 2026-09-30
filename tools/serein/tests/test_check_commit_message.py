import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

import check_commit_message  # noqa: E402

BODY = ("Explain why the change is needed and what it does, in a body that is\n"
        "long enough for the reviewers who read the history later.\n")


class CheckCommitMessageTest(unittest.TestCase):

    def check(self, message):
        return check_commit_message.problems(message)

    def test_accepts_conventional_message(self):
        self.assertEqual(self.check("feat(menu): select a range\n\n" + BODY), [])
        self.assertEqual(self.check("fix!: drop the old flag\n\n" + BODY), [])

    def test_rejects_long_header(self):
        header = "feat(history): keep expired self-destructing media and auto-deleted messages"
        found = self.check(header + "\n\n" + BODY)
        self.assertEqual(found, ["header is 76 characters, the limit is 72"])

    def test_rejects_header_shape_and_subject_rules(self):
        self.assertIn("header must look like 'type(scope): subject'",
                      self.check("Add a feature\n\n" + BODY))
        self.assertIn("type 'feature' is not one of build, chore, ci, docs, feat, "
                      "fix, perf, refactor, revert, style, test",
                      self.check("feature: add\n\n" + BODY))
        self.assertIn("subject must not end with a full stop",
                      self.check("feat: add a thing.\n\n" + BODY))
        self.assertIn("subject must not start with a capital letter",
                      self.check("feat: Add a thing\n\n" + BODY))
        self.assertIn("scope must be lower case",
                      self.check("feat(Menu): add\n\n" + BODY))

    def test_rejects_missing_short_and_wide_bodies(self):
        self.assertIn("a blank line and a body must follow the header",
                      self.check("feat: add a thing\n"))
        self.assertIn("body is 5 characters, the minimum is 60",
                      self.check("feat: add a thing\n\nshort\n"))
        wide = "x" * 101
        self.assertIn("line 3 is 101 characters, the limit is 100",
                      self.check("feat: add a thing\n\n" + wide + "\n" + BODY))

    def test_rejects_non_ascii_and_ignores_comments(self):
        self.assertIn("message must be printable ASCII (English only)",
                      self.check("feat: add a thing\n\n" + BODY + "中文\n"))
        self.assertEqual(self.check(
            "feat: add a thing\n\n" + BODY + "# 中文 comment from git\n"), [])


if __name__ == "__main__":
    unittest.main()
