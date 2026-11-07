"""Keep SereinGram settings sections safe to build for the settings search.

The settings search builds every section with a search context, in which
SectionBuilder::controller() and container() return null. A section may
dereference them only inside callbacks that run later, which capture by
value; code that runs while the section builds must use builder.session().
"""

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SOURCES = Path("Telegram/SourceFiles/serein")
BUILDER = re.compile(r"\bSectionBuilder\s*&\s*builder\b")
BOUND = re.compile(r"\b(\w+)\s*=\s*builder\s*\.\s*(?:controller|container)\s*\(\s*\)")
DIRECT = re.compile(r"\bbuilder\s*\.\s*(?:controller|container)\s*\(\s*\)\s*->")
LAMBDA = re.compile(
    r"\[([^\[\]]*)\]\s*(?:\([^()]*\))?\s*(?:mutable\s*)?(?:->\s*[\w:<>*&\s]+?)?\s*\{"
)


def blank(text):
    """Replace comments and literals with spaces, keeping every offset."""
    out = list(text)
    i, size = 0, len(text)
    while i < size:
        if text.startswith("//", i):
            end = text.find("\n", i)
            end = size if end < 0 else end
        elif text.startswith("/*", i):
            end = text.find("*/", i + 2)
            end = size if end < 0 else end + 2
        elif text[i] in "\"'":
            quote, end = text[i], i + 1
            while end < size and text[end] != quote and text[end] != "\n":
                end += 2 if text[end] == "\\" else 1
            end = min(end + 1, size)
        else:
            i += 1
            continue
        for j in range(i, end):
            if out[j] != "\n":
                out[j] = " "
        i = end
    return "".join(out)


def closing(text, start, opening="{", ending="}"):
    """Offset of the bracket that closes the one at start, or -1."""
    depth = 0
    for index in range(start, len(text)):
        if text[index] == opening:
            depth += 1
        elif text[index] == ending:
            depth -= 1
            if depth == 0:
                return index
    return -1


def bodies(text):
    """The bodies of functions and lambdas that take the section builder."""
    for match in BUILDER.finditer(text):
        open_paren = text.rfind("(", 0, match.start())
        close_paren = closing(text, open_paren, "(", ")") if open_paren >= 0 else -1
        if close_paren < match.end():
            continue
        rest = re.match(r"[^{;]*", text[close_paren + 1 :])
        brace = close_paren + 1 + rest.end()
        if brace < len(text) and text[brace] == "{":
            end = closing(text, brace)
            if end > 0:
                yield brace, end


def deferred(capture):
    capture = capture.strip()
    return bool(capture) and not capture.startswith("&")


def problems(path, source):
    text = blank(source)
    found = []
    for start, end in bodies(text):
        body = text[start:end]
        lambdas = []
        for match in LAMBDA.finditer(body):
            open_brace = start + match.end() - 1
            lambdas.append((open_brace, closing(text, open_brace), match.group(1)))
        names = {match.group(1) for match in BOUND.finditer(body)}
        uses = [start + match.start() for match in DIRECT.finditer(body)]
        for name in names:
            pattern = re.compile(r"\b" + re.escape(name) + r"\s*->")
            uses += [start + match.start() for match in pattern.finditer(body)]
        for use in uses:
            if not any(
                first < use < last and deferred(capture) for first, last, capture in lambdas
            ):
                line = text.count("\n", 0, use) + 1
                found.append(f"{path}:{line}: dereferences the window while the section builds")
    return sorted(set(found))


def main(argv=None):
    root = Path(argv[0]) if argv else ROOT
    files = sorted((root / SOURCES).rglob("*.cpp"))
    found = []
    for path in files:
        if "/gen/" in path.as_posix() or "/tests/" in path.as_posix():
            continue
        found += problems(path.relative_to(root), path.read_text(encoding="utf-8"))
    for problem in found:
        print(problem, file=sys.stderr)
    if found:
        print(
            f"{len(found)} settings builder problem(s): use builder.session() while "
            "building and the controller only inside callbacks that capture by value.",
            file=sys.stderr,
        )
        return 1
    print(f"Checked {len(files)} files; settings sections build without a window.")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
