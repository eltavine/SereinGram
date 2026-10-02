import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

import check_hook_namespaces


def write(root, path, text):
    target = Path(root) / path
    target.parent.mkdir(parents=True, exist_ok=True)
    target.write_text(text)


FACADE = """#pragma once
namespace Serein::Hooks::Ghost {
[[nodiscard]] bool GhostMode();
} // namespace Serein::Hooks::Ghost
"""


class CheckHookNamespacesTest(unittest.TestCase):
    def test_flags_shadowed_page_names(self):
        with tempfile.TemporaryDirectory() as root:
            write(root, "serein/hooks/gen/ghost.h", FACADE)
            write(root, "serein/hooks/a.h", '#include "serein/hooks/gen/ghost.h"\n')
            write(
                root,
                "serein/hooks/a.cpp",
                (
                    '#include "serein/hooks/a.h"\n'
                    "namespace Serein::Hooks {\n"
                    "bool A() { return Ghost::GhostMode() && Ghost::Allows(); }\n"
                    "} // namespace Serein::Hooks\n"
                ),
            )
            found = check_hook_namespaces.problems(root)
            self.assertEqual(len(found), 1)
            self.assertIn("serein/hooks/a.cpp:3: Ghost::Allows", found[0])

    def test_accepts_qualified_names_and_unrelated_files(self):
        with tempfile.TemporaryDirectory() as root:
            write(root, "serein/hooks/gen/ghost.h", FACADE)
            write(
                root,
                "serein/hooks/b.cpp",
                (
                    '#include "serein/hooks/gen/ghost.h"\n'
                    "namespace Serein::Hooks {\n"
                    "bool B() { return Serein::Ghost::Allows(); }\n"
                    "} // namespace Serein::Hooks\n"
                ),
            )
            write(
                root,
                "serein/app/c.cpp",
                (
                    "namespace Serein::App {\n"
                    "bool C() { return Ghost::Allows(); }\n"
                    "} // namespace Serein::App\n"
                ),
            )
            self.assertEqual(check_hook_namespaces.problems(root), [])

    def test_flags_unqualified_history_class_in_facades(self):
        with tempfile.TemporaryDirectory() as root:
            write(root, "serein/hooks/gen/ghost.h", FACADE)
            write(
                root,
                "serein/hooks/send.h",
                (
                    "class History;\n"
                    "namespace Serein::Hooks {\n"
                    "void A(gsl::not_null<History*> history);\n"
                    "void B(gsl::not_null<::History*> history);\n"
                    "} // namespace Serein::Hooks\n"
                ),
            )
            found = check_hook_namespaces.problems(root)
            self.assertEqual(len(found), 1)
            self.assertIn("serein/hooks/send.h:3: write ::History", found[0])

    def test_repository_sources_are_clean(self):
        root = Path(__file__).resolve().parents[3] / "Telegram/SourceFiles"
        self.assertEqual(check_hook_namespaces.problems(root), [])


if __name__ == "__main__":
    unittest.main()
