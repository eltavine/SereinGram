#!/usr/bin/env python3
"""Flag code inside Serein::Hooks that reaches a Serein::<Page> name
through a page prefix that a generated facade namespace shadows."""

import argparse
import re
import sys
from pathlib import Path

INCLUDE = re.compile(r'#include "([^"]+)"')
FACADE = re.compile(r"namespace Serein::Hooks::(\w+) \{")
DECLARED = re.compile(r"\b(\w+)\(")


def facades(root):
    result = {}
    for header in sorted((root / "serein/hooks/gen").glob("*.h")):
        text = header.read_text(encoding="utf-8")
        match = FACADE.search(text)
        if match:
            path = str(header.relative_to(root))
            result[match.group(1)] = (path, set(DECLARED.findall(text)))
    return result


def closure(root, path, seen):
    if path in seen or not (root / path).exists():
        return seen
    seen.add(path)
    for include in INCLUDE.findall((root / path).read_text(encoding="utf-8")):
        if include.startswith("serein/"):
            closure(root, include, seen)
    return seen


HISTORY_TYPE = re.compile(r"(?<![\w:])History\s*[*&>]")


def problems(root):
    root = Path(root)
    known = facades(root)
    result = []
    for header in sorted((root / "serein/hooks").rglob("*.h")):
        relative = str(header.relative_to(root))
        for number, line in enumerate(header.read_text(encoding="utf-8").splitlines(), 1):
            if HISTORY_TYPE.search(line):
                result.append(
                    f"{relative}:{number}: write ::History, because the "
                    "Serein::History record namespace shadows the class"
                )
    for source in sorted((root / "serein").rglob("*")):
        if source.suffix not in (".cpp", ".h") or "/gen/" in str(source):
            continue
        text = source.read_text(encoding="utf-8")
        if "namespace Serein::Hooks" not in text:
            continue
        relative = str(source.relative_to(root))
        included = closure(root, relative, set())
        for page, (header, names) in known.items():
            if header not in included:
                continue
            pattern = re.compile(r"(?<![\w:])" + page + r"::(\w+)")
            for number, line in enumerate(text.splitlines(), 1):
                for match in pattern.finditer(line):
                    if match.group(1) not in names:
                        result.append(
                            f"{relative}:{number}: {page}::{match.group(1)} "
                            f"resolves to Serein::Hooks::{page} from "
                            f"{header}; write Serein::{page}::"
                            f"{match.group(1)} instead"
                        )
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", default="Telegram/SourceFiles")
    args = parser.parse_args()
    found = problems(args.root)
    for problem in found:
        print(problem, file=sys.stderr)
    if found:
        return 1
    print("Hook namespaces OK.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
