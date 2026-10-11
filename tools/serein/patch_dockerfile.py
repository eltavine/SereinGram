"""Adapt the generated upstream Linux Dockerfile to an arm64 build.

The combined Rust archive uses the container's native target. On arm64
rnnoise v0.2 needs its upstream commit 372f7b4b76: the
release's NEON header includes os_support.h, which the release does not ship.
Each change fails loudly when the upstream text it relies on moves.
"""

import argparse
import sys
from pathlib import Path

RUST_STAGE = "FROM patches AS tdesktop_rust\n"
RNNOISE_STAGE = "cd rnnoise\n"
RNNOISE_FIX = (
    'sed -i -e \'s|#include "os_support.h"|#include "opus_types.h"\\n#include "common.h"|\' '
    "-e 's/OPUS_CLEAR(/RNN_CLEAR(/' src/vec_neon.h\n"
)


class PatchError(Exception):
    pass


def patch(text):
    if RNNOISE_FIX in text:
        raise PatchError("the rnnoise fix is already applied")
    if text.count(RUST_STAGE) != 1:
        raise PatchError("the combined Rust stage changed")
    rust = text.split(RUST_STAGE, 1)[1].split("\nFROM ", 1)[0]
    if "--target " in rust or "target/release/libtdesktop_rust.a" not in rust:
        raise PatchError("the combined Rust stage no longer uses the native target")
    if text.count(RNNOISE_STAGE) != 1:
        raise PatchError("the rnnoise stage changed")
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
