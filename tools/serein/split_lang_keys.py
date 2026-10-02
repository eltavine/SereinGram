"""Split the generated Lang::GetKeyIndex into one function per key letter.

codegen_lang emits the key lookup as a single function. With the SereinGram
strings merged in, MSVC for arm64 rejects it as too large (C1053), so every
top-level case of the switch on the first letter after "lng_" moves into its
own function. The lookup itself is unchanged. Text that does not have the
expected shape is copied as is.
"""

import sys
from pathlib import Path

HEAD = "ushort GetKeyIndex(QLatin1String key) {"
CASE = "\t\tcase '"
BREAK = "\t\tbreak;"


def split(text):
    lines = text.replace("\r\n", "\n").split("\n")
    try:
        start = lines.index(HEAD)
        end = lines.index("}", start)
    except ValueError:
        return text
    helpers = []
    dispatch = []
    label = None
    body = []
    count = 0
    for line in lines[start:end]:
        if label is None:
            if line.startswith(CASE) and line.endswith(":"):
                label, body = line, []
            else:
                dispatch.append(line)
        elif line == BREAK:
            name = f"GetKeyIndexPart{count}"
            count += 1
            helpers += [
                f"ushort {name}(",
                "\t\t[[maybe_unused]] qsizetype size,",
                "\t\t[[maybe_unused]] const char *data) {",
                *body,
                "\treturn kKeysCount;",
                "}",
                "",
            ]
            dispatch.append(f"{label} return {name}(size, data);")
            label = None
        else:
            body.append(line)
    if label is not None or not count:
        return text
    return "\n".join(
        [
            *lines[:start],
            "namespace {",
            "",
            *helpers,
            "} // namespace",
            "",
            *dispatch,
            *lines[end:],
        ]
    )


def main(argv=None):
    args = sys.argv[1:] if argv is None else argv
    if len(args) != 2:
        print("usage: split_lang_keys.py <lang_auto.cpp> <output>", file=sys.stderr)
        return 2
    source, target = Path(args[0]), Path(args[1])
    result = split(source.read_text(encoding="utf-8"))
    target.write_text(result, encoding="utf-8", newline="\n")
    return 0


if __name__ == "__main__":
    sys.exit(main())
