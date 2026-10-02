import contextlib
import io
import json
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

import check_boundaries

POLICY = {
    "schema_version": 1,
    "source_root": "src/",
    "app_prefixes": ["styles/"],
    "modules": {
        "serein/schema/": {
            "own": ["serein/schema/", "serein/core/options.h"],
            "app": False,
            "libraries": True,
        },
        "serein/ports/": {
            "own": ["serein/ports/", "serein/schema/"],
            "app": False,
            "libraries": False,
        },
        "serein/": {"own": ["serein/"], "app": True, "libraries": True},
    },
}


class CheckBoundariesTest(unittest.TestCase):
    def setUp(self):
        self._temp = tempfile.TemporaryDirectory()
        self.root = Path(self._temp.name)
        subprocess.run(["git", "init", "-q"], cwd=self.root, check=True)
        self.write("src/data/data_session.h", "")
        self.policy = self.root / "policy.json"
        self.policy.write_text(json.dumps(POLICY), encoding="utf-8")

    def tearDown(self):
        self._temp.cleanup()

    def write(self, name, text):
        path = self.root / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(text, encoding="utf-8")

    def run_check(self):
        output = io.StringIO()
        with contextlib.redirect_stdout(output), contextlib.redirect_stderr(output):
            code = check_boundaries.main(["--root", str(self.root), "--policy", str(self.policy)])
        return code, output.getvalue()

    def test_allowed_includes_pass(self):
        self.write(
            "src/serein/schema/a.h",
            '#include "serein/schema/b.h"\n#include "serein/core/options.h"\n'
            '#include "base/basic_types.h"\n#include <QtCore/QString>\n',
        )
        self.write("src/serein/ports/p.h", '#include "serein/schema/a.h"\n')
        self.write("src/serein/chats/legacy.cpp", '#include "data/data_session.h"\n')
        code, output = self.run_check()
        self.assertEqual(code, 0, output)

    def test_layers_may_not_include_other_modules(self):
        self.write("src/serein/ports/p.h", '#include "serein/adapters/x.h"\n')
        code, output = self.run_check()
        self.assertEqual(code, 1)
        self.assertIn('serein/ports/p.h: includes "serein/adapters/x.h"', output)

    def test_exact_file_allowance_does_not_open_the_directory(self):
        self.write("src/serein/schema/a.h", '#include "serein/core/registry.h"\n')
        code, _ = self.run_check()
        self.assertEqual(code, 1)

    def test_upstream_headers_are_detected_by_existence(self):
        self.write("src/serein/schema/a.h", '#include "data/data_session.h"\n')
        code, output = self.run_check()
        self.assertEqual(code, 1)
        self.assertIn("may not know upstream code", output)

    def test_generated_styles_count_as_upstream(self):
        self.write("src/serein/schema/a.h", '#include "styles/style_chat.h"\n')
        code, _ = self.run_check()
        self.assertEqual(code, 1)

    def test_libraries_can_be_forbidden(self):
        self.write("src/serein/ports/p.h", '#include "base/basic_types.h"\n')
        code, output = self.run_check()
        self.assertEqual(code, 1)
        self.assertIn("may not use libraries", output)

    def test_rejects_malformed_policy(self):
        broken = dict(POLICY, modules={"serein": {"own": [], "app": True, "libraries": True}})
        self.policy.write_text(json.dumps(broken), encoding="utf-8")
        code, _ = self.run_check()
        self.assertEqual(code, 2)


if __name__ == "__main__":
    unittest.main()
