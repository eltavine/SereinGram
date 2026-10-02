#!/usr/bin/env python3
"""Check that every Serein source file is registered in CMake and back.

A .cpp file missing from serein.cmake only shows up as undefined
references when the platform workflows link the app, and a listed path
that was renamed only fails there too.
"""

import argparse
import re
import sys
from pathlib import Path

LISTS = (
    "Telegram/cmake/serein.cmake",
    "Telegram/cmake/serein_tests.cmake",
    "Telegram/SourceFiles/serein/schema/gen/sources.cmake",
)
SOURCE = re.compile(r"(serein/[A-Za-z0-9_/.\-]+\.(?:cpp|mm|swift))")
EXTENSIONS = {".cpp", ".mm", ".swift"}


def listed(root):
    result = set()
    for name in LISTS:
        path = root / name
        if path.is_file():
            result.update(SOURCE.findall(path.read_text(encoding="utf-8")))
    return result


def existing(root):
    sources = root / "Telegram" / "SourceFiles"
    serein = sources / "serein"
    return {
        path.relative_to(sources).as_posix()
        for path in serein.rglob("*")
        if path.suffix in EXTENSIONS and path.is_file()
    }


def problems(root):
    registered = listed(root)
    present = existing(root)
    result = [f"listed but missing: {path}" for path in sorted(registered - present)]
    result += [f"not registered: {path}" for path in sorted(present - registered)]
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=Path.cwd())
    args = parser.parse_args()
    found = problems(args.root.resolve())
    for line in found:
        print(line, file=sys.stderr)
    if found:
        print(f"{len(found)} source registration problem(s).", file=sys.stderr)
        return 1
    print("Source registration OK.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
