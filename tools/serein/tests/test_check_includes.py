import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

import check_includes


class CheckIncludesTest(unittest.TestCase):
    def setUp(self):
        self._temp = tempfile.TemporaryDirectory()
        self.root = Path(self._temp.name)
        self.write("Telegram/SourceFiles/data/data_peer.h", "")
        self.write("Telegram/lib_ui/ui/painter.h", "")
        self.write("Telegram/ThirdParty/OpenCC/src/SimpleConverter.hpp", "")
        self.write("Telegram/SourceFiles/serein/feature/local.h", "")

    def tearDown(self):
        self._temp.cleanup()

    def write(self, name, text):
        path = self.root / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(text, encoding="utf-8")

    def test_existing_and_generated_headers_pass(self):
        self.write(
            "Telegram/SourceFiles/serein/feature/code.cpp",
            "\n".join(
                [
                    '#include "data/data_peer.h"',
                    '#include "ui/painter.h"',
                    '#include "SimpleConverter.hpp"',
                    '#include "local.h"',
                    '#include "styles/style_serein.h"',
                    '#include "serein/hooks/gen/chats.h"',
                    "#include <QtCore/QString>",
                ]
            ),
        )
        self.assertEqual(check_includes.problems(self.root), [])

    def test_missing_header_is_reported(self):
        self.write(
            "Telegram/SourceFiles/serein/feature/code.cpp", '#include "main/main_session_show.h"\n'
        )
        self.assertEqual(
            check_includes.problems(self.root),
            [
                "Telegram/SourceFiles/serein/feature/code.cpp: missing main/main_session_show.h",
            ],
        )

    def test_upstream_files_only_check_serein_includes(self):
        self.write(
            "Telegram/SourceFiles/boxes/box.cpp",
            "\n".join(
                [
                    '#include "not/checked.h"',
                    '#include "serein/hooks/missing.h"',
                ]
            ),
        )
        self.assertEqual(
            check_includes.problems(self.root),
            [
                "Telegram/SourceFiles/boxes/box.cpp: missing serein/hooks/missing.h",
            ],
        )

    def test_banned_system_header_is_reported_in_serein_only(self):
        self.write("Telegram/SourceFiles/serein/feature/code.cpp", "#include <filesystem>\n")
        self.write("Telegram/SourceFiles/boxes/box.cpp", "#include <filesystem>\n")
        self.assertEqual(
            check_includes.problems(self.root),
            [
                "Telegram/SourceFiles/serein/feature/code.cpp: <filesystem> "
                "std::filesystem needs macOS 10.15; use QDir and QFileInfo",
            ],
        )


if __name__ == "__main__":
    unittest.main()
