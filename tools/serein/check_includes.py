#!/usr/bin/env python3
"""Check that quoted includes in Serein code name headers that exist.

The app only compiles in CI, so a mistyped include path costs a full
platform build. Generated headers are skipped because they only exist
in the build tree.
"""

import argparse
import re
import sys
from pathlib import Path

INCLUDE = re.compile(r'^\s*#\s*include\s+"([^"]+)"', re.M)
GENERATED = (
    "styles/",
    "lang_auto",
    "serein/schema/gen/",
    "serein/hooks/gen/",
    "serein/settings/gen/",
)
EXTENSIONS = {".h", ".cpp", ".mm"}


def include_roots(root):
    telegram = root / "Telegram"
    roots = [telegram / "SourceFiles", telegram / "ThirdParty" / "OpenCC" / "src"]
    roots.extend(sorted(path for path in telegram.glob("lib_*") if path.is_dir()))
    return roots


def resolves(include, source, roots):
    if include.startswith(GENERATED):
        return True
    candidates = [source.parent / include] + [root / include for root in roots]
    return any(candidate.is_file() for candidate in candidates)


def sources(root):
    serein = root / "Telegram" / "SourceFiles" / "serein"
    for path in sorted(serein.rglob("*")):
        if path.suffix in EXTENSIONS and path.is_file():
            yield path, None
    upstream = root / "Telegram" / "SourceFiles"
    for path in sorted(upstream.rglob("*")):
        if path.suffix not in EXTENSIONS or not path.is_file():
            continue
        if serein in path.parents:
            continue
        yield path, "serein/"


def problems(root):
    roots = include_roots(root)
    result = []
    for path, prefix in sources(root):
        text = path.read_text(encoding="utf-8", errors="replace")
        for include in INCLUDE.findall(text):
            if prefix and not include.startswith(prefix):
                continue
            if not resolves(include, path, roots):
                result.append(f"{path.relative_to(root)}: missing {include}")
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=Path.cwd())
    args = parser.parse_args()
    found = problems(args.root.resolve())
    for line in found:
        print(line, file=sys.stderr)
    if found:
        print(f"{len(found)} include(s) name missing headers.", file=sys.stderr)
        return 1
    print("Includes OK.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
