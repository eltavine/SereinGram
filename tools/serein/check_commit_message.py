#!/usr/bin/env python3
"""Check a commit message against the rules of tools/serein/commitlint.config.mjs."""

import re
import sys
from pathlib import Path

TYPES = (
    "build",
    "chore",
    "ci",
    "docs",
    "feat",
    "fix",
    "perf",
    "refactor",
    "revert",
    "style",
    "test",
)
HEADER = re.compile(r"^(?P<type>[a-z]+)(\((?P<scope>[^()]+)\))?(?P<breaking>!)?: (?P<subject>.+)$")
PRINTABLE = re.compile(r"^[\t\n\r\x20-\x7e]*$")


def problems(message):
    lines = [line for line in message.splitlines() if not line.startswith("#")]
    while lines and not lines[-1].strip():
        lines.pop()
    if not lines:
        return ["message is empty"]
    result = []
    header = lines[0]
    if len(header) > 72:
        result.append(f"header is {len(header)} characters, the limit is 72")
    match = HEADER.match(header)
    if not match:
        result.append("header must look like 'type(scope): subject'")
    else:
        if match["type"] not in TYPES:
            result.append(f"type '{match['type']}' is not one of {', '.join(TYPES)}")
        scope = match["scope"]
        if scope is not None and scope != scope.lower():
            result.append("scope must be lower case")
        subject = match["subject"]
        if subject.endswith("."):
            result.append("subject must not end with a full stop")
        if subject[:1].isupper():
            result.append("subject must not start with a capital letter")
    if len(lines) < 3 or lines[1].strip():
        result.append("a blank line and a body must follow the header")
    body = "\n".join(lines[2:])
    if len(body) < 60:
        result.append(f"body is {len(body)} characters, the minimum is 60")
    for number, line in enumerate(lines[2:], 3):
        if len(line) > 100:
            result.append(f"line {number} is {len(line)} characters, the limit is 100")
    if not PRINTABLE.match("\n".join(lines)):
        result.append("message must be printable ASCII (English only)")
    return result


def main():
    if len(sys.argv) != 2:
        print("usage: check_commit_message.py MESSAGE_FILE", file=sys.stderr)
        return 2
    found = problems(Path(sys.argv[1]).read_text(encoding="utf-8"))
    for problem in found:
        print(f"commit message: {problem}", file=sys.stderr)
    return 1 if found else 0


if __name__ == "__main__":
    sys.exit(main())
