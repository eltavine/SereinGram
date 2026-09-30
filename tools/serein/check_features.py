#!/usr/bin/env python3
"""Check that the feature matrix only uses the IDs, statuses, priorities
and sources that docs/serein/README.md defines."""

import argparse
import re
import sys
from pathlib import Path

STATUSES = {"Planned", "In Progress", "Implemented", "Verified"}
PRIORITIES = {"P0", "P1", "P2", "P3"}
SOURCES = {"Ni", "Na", "Ad", "Aa", "T", "D"}
ID = re.compile(r"SG-[A-Z]+-\d{2}")


def parse(text):
    rows = []
    for number, line in enumerate(text.splitlines(), 1):
        if not line.startswith("| SG-"):
            continue
        cells = [cell.strip() for cell in line.strip().strip("|").split("|")]
        if len(cells) < 5:
            rows.append((number, None, None, None, None))
            continue
        rows.append((number, cells[0], cells[-3], cells[-2], cells[-1]))
    return rows


def problems(text):
    result = []
    seen = {}
    for number, id, source, status, priority in parse(text):
        where = f"line {number}"
        if id is None:
            result.append(f"{where}: expected ID, feature, source, status and priority cells")
            continue
        if not ID.fullmatch(id):
            result.append(f"{where}: malformed ID {id!r}")
        if id in seen:
            result.append(f"{where}: {id} already used on line {seen[id]}")
        seen.setdefault(id, number)
        if status not in STATUSES:
            result.append(f"{where}: {id} has undefined status {status!r}")
        if priority not in PRIORITIES:
            result.append(f"{where}: {id} has undefined priority {priority!r}")
        unknown = set(source.split()) - SOURCES
        if not source.split() or unknown:
            result.append(f"{where}: {id} has undefined source {source!r}")
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--matrix", default="docs/serein/features.md")
    args = parser.parse_args()
    found = problems(Path(args.matrix).read_text(encoding="utf-8"))
    for problem in found:
        print(problem, file=sys.stderr)
    if found:
        return 1
    print(f"Feature matrix OK: {len(parse(Path(args.matrix).read_text(encoding='utf-8')))} rows.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
