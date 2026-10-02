import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

import check_sources


class CheckSourcesTest(unittest.TestCase):
    def setUp(self):
        self._temp = tempfile.TemporaryDirectory()
        self.root = Path(self._temp.name)
        self.write("Telegram/SourceFiles/serein/feature/code.cpp", "")
        self.write("Telegram/SourceFiles/serein/tests/test_code.cpp", "")
        self.write("Telegram/SourceFiles/serein/schema/gen/config/x.cpp", "")
        self.write("Telegram/SourceFiles/serein/feature/code.h", "")

    def tearDown(self):
        self._temp.cleanup()

    def write(self, name, text):
        path = self.root / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(text, encoding="utf-8")

    def register(self, app, tests=(), generated=()):
        self.write("Telegram/cmake/serein.cmake", "\n".join(f"    {path}" for path in app))
        self.write("Telegram/cmake/serein_tests.cmake", "\n".join(f"    {path}" for path in tests))
        self.write(
            "Telegram/SourceFiles/serein/schema/gen/sources.cmake",
            "\n".join(f"    {path}" for path in generated),
        )

    def test_registered_sources_pass(self):
        self.register(
            ["serein/feature/code.cpp"],
            ["serein/tests/test_code.cpp"],
            ["serein/schema/gen/config/x.cpp"],
        )
        self.assertEqual(check_sources.problems(self.root), [])

    def test_unregistered_and_missing_sources_are_reported(self):
        self.register(
            ["serein/feature/renamed.cpp"],
            ["serein/tests/test_code.cpp"],
            ["serein/schema/gen/config/x.cpp"],
        )
        self.assertEqual(
            check_sources.problems(self.root),
            [
                "listed but missing: serein/feature/renamed.cpp",
                "not registered: serein/feature/code.cpp",
            ],
        )


if __name__ == "__main__":
    unittest.main()
