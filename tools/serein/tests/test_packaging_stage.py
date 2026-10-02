import os
import stat
import subprocess
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
STAGE = ROOT / "packaging/nfpm/stage.sh"
CONFIG = ROOT / "packaging/nfpm/nfpm.yaml"
ID = "io.github.eltavine.SereinGram"


class PackagingStageTest(unittest.TestCase):
    def setUp(self):
        self._temp = tempfile.TemporaryDirectory()
        self.temp = Path(self._temp.name)
        self.binary = self.temp / "SereinGram"
        self.binary.write_text("#!/bin/sh\n", encoding="utf-8")
        self.stage = self.temp / "stage"
        subprocess.run(
            [
                "bash",
                str(STAGE),
                str(self.binary),
                str(ROOT / f"lib/xdg/{ID}.metainfo.xml"),
                str(self.stage),
            ],
            check=True,
            capture_output=True,
            text=True,
        )

    def tearDown(self):
        self._temp.cleanup()

    def test_lays_out_every_packaged_file(self):
        expected = [
            "SereinGram",
            f"{ID}.desktop",
            f"{ID}.metainfo.xml",
            f"icons/symbolic/apps/{ID}-symbolic.svg",
        ]
        for size in (16, 32, 48, 64, 128, 256, 512):
            for scale in ("", "@2"):
                expected.append(f"icons/{size}x{size}{scale}/apps/{ID}.png")
        for name in expected:
            self.assertTrue((self.stage / name).is_file(), name)
        mode = (self.stage / "SereinGram").stat().st_mode
        self.assertTrue(mode & stat.S_IXOTH)

    def test_desktop_entry_needs_no_dbus_service(self):
        entry = (self.stage / f"{ID}.desktop").read_text(encoding="utf-8")
        self.assertNotIn("DBusActivatable", entry)
        self.assertIn("Exec=SereinGram", entry)

    def test_config_packages_only_staged_paths(self):
        sources = [
            line.split("src:", 1)[1].strip()
            for line in CONFIG.read_text(encoding="utf-8").splitlines()
            if line.strip().startswith("- src:")
        ]
        self.assertTrue(sources)
        for source in sources:
            self.assertTrue(source.startswith("./"), source)
            self.assertTrue(os.path.exists(self.stage / source[2:]), source)


if __name__ == "__main__":
    unittest.main()
