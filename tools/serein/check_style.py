"""Check the formatting and style rules of SereinGram-owned files.

Every owned text file must be UTF-8 without a byte order mark, use LF line
endings, end with exactly one newline and carry no trailing whitespace.
SereinGram C++ additionally follows the mechanical rules of AGENTS.md and
REVIEW.md that a formatter cannot express: tab indentation, single empty
lines, leading operators on continuation lines, an empty line before the end
of a class with access sections, nested namespace syntax, no [[nodiscard]]
on out-of-class definitions, rationed comments and a list of banned
constructs.
"""

import argparse
import json
import re
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
TEXT_PATTERNS = (
    "Telegram/SourceFiles/serein/**/*",
    "Telegram/cmake/serein*.cmake",
    "Telegram/Resources/langs/serein/*",
    "proto/**/*",
    "tools/serein/**/*",
    "packaging/**/*",
    "flake.nix",
    "docs/serein/**/*",
    ".github/workflows/serein-*.yml",
    "lib/xdg/io.github.eltavine.SereinGram.*",
    "BRANDING.md",
    "crowdin.yml",
)
SKIPPED = re.compile(r"(^|/)(gen|__pycache__|\.venv|node_modules)/")
BINARY = {".png", ".ico", ".icns", ".svg", ".lock"}
CPP = {".cpp", ".h", ".mm"}
SPACE_INDENTED = {".py", ".yml", ".yaml", ".json", ".proto", ".cmake", ".toml", ".mjs"}
TESTS = re.compile(r"(^|/)tests/")
CANONICAL_JSON = "tools/serein/policy/"
BANNED = (
    (re.compile(r"\bQStringLiteral\("), 'use u"..."_q instead of QStringLiteral'),
    (re.compile(r"static_cast<void>\("), "do not discard results with a cast"),
    (re.compile(r"^\s*\(void\)\s*[\w(]"), "do not discard results with a cast"),
    (re.compile(r"\bQ_OS_LINUX\b"), "use !defined Q_OS_WIN && !defined Q_OS_MAC"),
    (re.compile(r"\bNULL\b"), "use nullptr"),
)
DEBUG_ONLY = re.compile(r"^\s*#\s*(ifdef|ifndef|if\s+defined)\s*\(?\s*_DEBUG")
CLASS = re.compile(r"^\s*(?:template\s*<.*>\s*)?class\s+\w[^;]*\{\s*$")
ACCESS = re.compile(r"^\s*(public|protected|private)\s*(slots\s*)?:\s*$")
NAMED_NAMESPACE = re.compile(r"^\s*namespace\s+[\w:]+\s*\{\s*$")
NODISCARD_DEFINITION = re.compile(r"^\s*\[\[nodiscard\]\].*\b[A-Z]\w*(?:<[^>]*>)?::~?\w+\(")
TRAILING_OPERATOR = re.compile(r"(&&|\|\|)\s*$")
COMMENT = re.compile(r"^\s*//")


def owned_files(root):
    tracked = subprocess.run(
        ["git", "ls-files", "-z", "--", *TEXT_PATTERNS], cwd=root, check=True, capture_output=True
    ).stdout.decode()
    for name in sorted(filter(None, tracked.split("\0"))):
        path = Path(name)
        if SKIPPED.search(name) or path.suffix in BINARY:
            continue
        if (Path(root) / path).is_file():
            yield path


def strip_strings(line):
    line = re.sub(r'R"\(.*?\)"', '""', line)
    line = re.sub(r'"(?:\\.|[^"\\])*"', '""', line)
    return re.sub(r"'(?:\\.|[^'\\])*'", "''", line)


def text_errors(path, data):
    if data.startswith(b"\xef\xbb\xbf"):
        yield 1, "starts with a UTF-8 byte order mark"
    try:
        text = data.decode("utf-8")
    except UnicodeDecodeError as error:
        yield 1, f"is not UTF-8 ({error.reason})"
        return
    if "\r" in text:
        yield text[: text.index("\r")].count("\n") + 1, "has CR line endings"
    if text and not text.endswith("\n"):
        yield text.count("\n") + 1, "does not end with a newline"
    elif text.endswith("\n\n"):
        yield text.count("\n"), "ends with empty lines"
    if path.suffix == ".json":
        yield from json_errors(path, text)
    spaces_only = path.suffix in SPACE_INDENTED
    for number, line in enumerate(text.split("\n"), 1):
        if line != line.rstrip():
            yield number, "has trailing whitespace"
        if spaces_only and line.startswith("\t"):
            yield number, "is indented with a tab"


def json_errors(path, text):
    try:
        data = json.loads(text)
    except ValueError as error:
        yield getattr(error, "lineno", 1), f"is not valid JSON ({error})"
        return
    canonical = json.dumps(data, indent=2, ensure_ascii=False) + "\n"
    if path.as_posix().startswith(CANONICAL_JSON) and text != canonical:
        yield 1, "is not formatted like json.dumps(indent=2)"


def comment_errors(lines):
    block = []
    for number, line in enumerate([*lines, ""], 1):
        if COMMENT.match(line):
            block.append((number, line.strip()))
            continue
        if len(block) > 3:
            yield block[0][0], "comment block is longer than three lines"
        elif len(block) > 1 and not block[0][1].startswith("// WHY:"):
            yield block[0][0], ("multi-line comment must open with '// WHY:'")
        block = []


def class_errors(lines):
    stack = []
    for number, line in enumerate(lines, 1):
        code = strip_strings(line.split("//")[0])
        if CLASS.match(code):
            stack.append(["class", False])
            continue
        if stack and stack[-1][0] == "class" and ACCESS.match(code):
            stack[-1][1] = True
        opens = code.count("{")
        closes = code.count("}")
        if closes and stack:
            for _ in range(min(closes, len(stack))):
                kind, sections = stack.pop()
                if (
                    kind == "class"
                    and sections
                    and code.strip() == "};"
                    and lines[number - 2].strip()
                ):
                    yield number, ("class with access sections needs an empty line before '};'")
        stack += [["block", False]] * opens


def cpp_errors(path, lines):
    production = not TESTS.search(path.as_posix())
    previous_empty = False
    previous_namespace = False
    for number, line in enumerate(lines, 1):
        code = strip_strings(line)
        if line and line[0] == " " and not line.lstrip().startswith("*"):
            yield number, "is indented with spaces"
        empty = not line.strip()
        if empty and previous_empty:
            yield number, "follows another empty line"
        previous_empty = empty
        if COMMENT.match(line):
            continue
        bare = code.split("//")[0]
        for pattern, message in BANNED:
            if pattern.search(bare):
                yield number, message
        if production and DEBUG_ONLY.match(bare):
            yield number, "debug-only code belongs in the test harness"
        if TRAILING_OPERATOR.search(bare):
            yield number, "put '&&' or '||' at the start of the next line"
        namespace = bool(NAMED_NAMESPACE.match(bare))
        if namespace and previous_namespace:
            yield number, "use nested namespace syntax"
        previous_namespace = namespace
        if path.suffix == ".cpp" and NODISCARD_DEFINITION.match(bare):
            yield number, "[[nodiscard]] belongs on the declaration only"
    if production:
        yield from comment_errors(lines)
    yield from class_errors(lines)


def check(root, paths):
    failures = 0
    for path in paths:
        data = (Path(root) / path).read_bytes()
        errors = list(text_errors(path, data))
        if path.suffix in CPP and not errors:
            lines = data.decode("utf-8").split("\n")
            errors += list(cpp_errors(path, lines[:-1]))
        for number, message in sorted(errors):
            print(f"{path}:{number}: {message}")
        failures += len(errors)
    return failures


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--root", default=str(ROOT))
    parser.add_argument("paths", nargs="*")
    args = parser.parse_args(argv)
    paths = [Path(p) for p in args.paths] if args.paths else list(owned_files(args.root))
    failures = check(args.root, paths)
    if failures:
        print(f"{failures} style problems.", file=sys.stderr)
        return 1
    print(f"Checked the style of {len(paths)} owned files.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
