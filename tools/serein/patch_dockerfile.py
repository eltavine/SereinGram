"""Adapt the generated upstream Linux Dockerfile for Serein CI.

Rocky Linux 8's Expat 2.2.5 RPM lacks symbols used by Python 3.12. On arm64,
rnnoise v0.2 needs upstream commit 372f7b4b76 for its NEON header.
Each change fails loudly when the upstream text it relies on moves.
"""

import argparse
import sys
from pathlib import Path

BUILDER_PACKAGES = "\ndnf -y install cmake autoconf automake libtool pkgconfig make patch git \\\n"
EXPAT_PACKAGES = "\t'expat >= 2.5.0' python3.12 \\\n"
PYTHON_SETUP = "\nalternatives --set python3 /usr/bin/python3.11\n"
EXPAT_CHECK = "python3.12 -c 'import pyexpat'\n"
RUST_STAGE = "FROM patches AS tdesktop_rust\n"
RNNOISE_STAGE = "cd rnnoise\n"
RNNOISE_FIX = (
    'sed -i -e \'s|#include "os_support.h"|#include "opus_types.h"\\n#include "common.h"|\' '
    "-e 's/OPUS_CLEAR(/RNN_CLEAR(/' src/vec_neon.h\n"
)


class PatchError(Exception):
    pass


def patch(text, arch):
    if EXPAT_PACKAGES in text or EXPAT_CHECK in text:
        raise PatchError("the Expat fix is already applied")
    if text.count(BUILDER_PACKAGES) != 1 or text.count(PYTHON_SETUP) != 1:
        raise PatchError("the builder's DNF or Python setup changed")
    text = text.replace(BUILDER_PACKAGES, BUILDER_PACKAGES + EXPAT_PACKAGES)
    text = text.replace(PYTHON_SETUP, PYTHON_SETUP + EXPAT_CHECK)
    if arch == "x86_64":
        return text

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
    parser.add_argument("--arch", choices=("x86_64", "arm64"), required=True)
    args = parser.parse_args(argv)
    path = Path(args.dockerfile)
    try:
        path.write_text(patch(path.read_text(encoding="utf-8"), args.arch), encoding="utf-8")
    except PatchError as error:
        print(f"patch_dockerfile.py: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
