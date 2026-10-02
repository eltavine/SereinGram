"""Run clang-tidy on the SereinGram sources of a compilation database.

The checks come from Telegram/SourceFiles/serein/.clang-tidy. Only
diagnostics located in hand-written SereinGram code fail the run: generated
code is covered by its generator tests, and upstream headers such as the
lib_base assertion macros are not ours to change. A translation unit that
clang-tidy cannot parse fails the run as well.
"""

import argparse
import json
import re
import shlex
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

OWNED = "/Telegram/SourceFiles/serein/"
GENERATED = "/gen/"
DIAGNOSTIC = re.compile(
    r"^(?P<file>[^\s:][^:]*):(?P<line>\d+):(?P<column>\d+): "
    r"(?P<kind>warning|error): (?P<message>.*?)(?: \[(?P<check>[^\]]+)\])?$"
)


def owned(path):
    path = path.replace("\\", "/")
    return OWNED in path and GENERATED not in path.split(OWNED, 1)[1]


def sources(database):
    entries = json.loads(Path(database).read_text(encoding="utf-8"))
    files = {str(Path(entry["directory"], entry["file"])) for entry in entries}
    return sorted(path for path in files if owned(path) and path.endswith((".cpp", ".mm")))


def default_extra_args():
    result = ["-Wno-unknown-warning-option"]
    if sys.platform == "darwin":
        sdk = subprocess.run(
            ["xcrun", "--show-sdk-path"], capture_output=True, text=True, check=True
        ).stdout.strip()
        result.append(f"-isysroot{sdk}")
    return result


def run_one(command, build, extra, path):
    arguments = [*command, "-p", build, "--quiet"]
    arguments += [f"--extra-arg={argument}" for argument in extra]
    result = subprocess.run([*arguments, path], capture_output=True, text=True, check=False)
    return path, result.returncode, result.stdout + result.stderr


def findings(output):
    for line in output.splitlines():
        match = DIAGNOSTIC.match(line)
        if not match:
            continue
        if match["check"] == "clang-diagnostic-error" or owned(match["file"]):
            yield line


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("-p", "--build", required=True, help="directory with compile_commands.json")
    parser.add_argument("--clang-tidy", default="clang-tidy", help="command that runs clang-tidy")
    parser.add_argument("-j", "--jobs", type=int, default=8)
    parser.add_argument("--extra-arg", action="append", default=[])
    args = parser.parse_args(argv)
    files = sources(Path(args.build) / "compile_commands.json")
    if not files:
        print("No SereinGram sources in the compilation database.", file=sys.stderr)
        return 1
    command = shlex.split(args.clang_tidy)
    extra = default_extra_args() + args.extra_arg
    reported = set()
    failed = []
    with ThreadPoolExecutor(max_workers=args.jobs) as pool:
        runs = pool.map(lambda path: run_one(command, args.build, extra, path), files)
        for path, code, output in runs:
            lines = list(findings(output))
            if code != 0 and not lines:
                failed.append(path)
                print(output, file=sys.stderr)
            for line in lines:
                if line not in reported:
                    reported.add(line)
                    print(line.replace(str(Path.cwd()) + "/", ""))
    if reported or failed:
        print(
            f"clang-tidy: {len(reported)} findings, {len(failed)} files failed.",
            file=sys.stderr,
        )
        return 1
    print(f"clang-tidy: {len(files)} SereinGram translation units are clean.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
