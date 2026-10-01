#!/usr/bin/env python3
"""Check that distribution packaging pins the tdlib commit that the
upstream snap recipe builds tde2e from."""

import argparse
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SNAP = "snap/snapcraft.yaml"
PKGBUILD = "packaging/arch/PKGBUILD"
SNAP_COMMIT = re.compile(
    r"^  tde2e:\n(?:    .*\n)*?    source-commit: ([0-9a-f]{40})$", re.M)
PKGBUILD_COMMIT = re.compile(r"^_td_commit=([0-9a-f]{40})$", re.M)


def pinned(root, path, pattern):
    match = pattern.search((root / path).read_text(encoding="utf-8"))
    return match.group(1) if match else None


def problems(root):
    root = Path(root)
    snap = pinned(root, SNAP, SNAP_COMMIT)
    arch = pinned(root, PKGBUILD, PKGBUILD_COMMIT)
    if not snap:
        return [f"{SNAP} has no 40-character tde2e source-commit."]
    if not arch:
        return [f"{PKGBUILD} has no 40-character _td_commit."]
    if snap != arch:
        return [f"{PKGBUILD} pins tdlib {arch[:10]}, but {SNAP} builds "
                f"tde2e from {snap[:10]}; update _td_commit."]
    return []


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=ROOT)
    args = parser.parse_args(argv)
    found = problems(args.root)
    for problem in found:
        print(problem)
    if found:
        return 1
    print("Packaging pins the upstream tdlib commit.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
