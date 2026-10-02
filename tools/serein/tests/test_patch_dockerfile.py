import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

import patch_dockerfile

DOCKERFILE = """\
FROM builder AS rnnoise
RUN <<EOF
git clone -b v0.2 --depth=1 https://github.com/xiph/rnnoise.git
cd rnnoise
./autogen.sh
EOF

FROM builder AS tlottie
RUN <<EOF
cargo rustc --lib --release --target x86_64-unknown-linux-gnu
objcopy --strip-debug target/x86_64-unknown-linux-gnu/release/libtlottie.a out.a
EOF
"""

VEC_NEON = """\
#include <arm_neon.h>
#include "os_support.h"

static inline void clear(float *out, int rows) {
   OPUS_CLEAR(out, rows);
}
"""


def gnu_sed():
    result = subprocess.run(["sed", "--version"], capture_output=True, text=True, check=False)
    return result.returncode == 0 and "GNU" in result.stdout


class PatchDockerfileTest(unittest.TestCase):
    def test_switches_the_rust_target_and_fixes_rnnoise(self):
        text = patch_dockerfile.patch(DOCKERFILE)
        self.assertNotIn("x86_64-unknown-linux-gnu", text)
        self.assertEqual(text.count("aarch64-unknown-linux-gnu"), 2)
        self.assertIn("cd rnnoise\nsed -i ", text)
        self.assertLess(text.index("sed -i "), text.index("./autogen.sh"))

    @unittest.skipUnless(gnu_sed(), "the fix runs with GNU sed in the Rocky Linux image")
    def test_the_rnnoise_fix_matches_upstream(self):
        with tempfile.TemporaryDirectory() as temp:
            source = Path(temp) / "src"
            source.mkdir()
            (source / "vec_neon.h").write_text(VEC_NEON)
            subprocess.run(["bash", "-c", patch_dockerfile.RNNOISE_FIX], cwd=temp, check=True)
            fixed = (source / "vec_neon.h").read_text()
        self.assertIn('#include "opus_types.h"\n#include "common.h"\n', fixed)
        self.assertIn("RNN_CLEAR(out, rows);", fixed)
        self.assertNotIn("os_support.h", fixed)

    def test_fails_when_upstream_moves(self):
        with self.assertRaises(patch_dockerfile.PatchError):
            patch_dockerfile.patch(DOCKERFILE.replace("x86_64", "riscv64"))
        with self.assertRaises(patch_dockerfile.PatchError):
            patch_dockerfile.patch(DOCKERFILE.replace("cd rnnoise\n", ""))

    def test_main_rewrites_the_file(self):
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / "Dockerfile"
            path.write_text(DOCKERFILE)
            self.assertEqual(patch_dockerfile.main([str(path)]), 0)
            self.assertIn("aarch64-unknown-linux-gnu", path.read_text())
            self.assertEqual(patch_dockerfile.main([str(path)]), 1)


if __name__ == "__main__":
    unittest.main()
