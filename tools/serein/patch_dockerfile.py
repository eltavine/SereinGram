"""Adapt the generated upstream Linux Dockerfile to an arm64 build.

Upstream builds that image only on x86_64. On arm64 the tlottie stage needs the
aarch64 Rust target, and rnnoise v0.2 needs its upstream commit 372f7b4b76: the
release's NEON header includes os_support.h, which the release does not ship.
Each change fails loudly when the upstream text it relies on moves.
"""

import argparse
import sys
from pathlib import Path

RUST_TARGETS = ("x86_64-unknown-linux-gnu", "aarch64-unknown-linux-gnu")
RNNOISE_STAGE = "cd rnnoise\n"
RNNOISE_FIX = (
    'sed -i -e \'s|#include "os_support.h"|#include "opus_types.h"\\n#include "common.h"|\' '
    "-e 's/OPUS_CLEAR(/RNN_CLEAR(/' src/vec_neon.h\n"
)


class PatchError(Exception):
    pass


def patch(text):
    if RUST_TARGETS[0] not in text:
        raise PatchError("the tlottie stage no longer names the x86_64 Rust target")
    if text.count(RNNOISE_STAGE) != 1:
        raise PatchError("the rnnoise stage changed")
    text = text.replace(*RUST_TARGETS)
    return text.replace(RNNOISE_STAGE, RNNOISE_STAGE + RNNOISE_FIX)


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("dockerfile")
    args = parser.parse_args(argv)
    path = Path(args.dockerfile)
    try:
        path.write_text(patch(path.read_text(encoding="utf-8")), encoding="utf-8")
    except PatchError as error:
        print(f"patch_dockerfile.py: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
