#!/usr/bin/env python3
# /// script
# requires-python = ">=3.11"
# dependencies = ["jinja2==3.1.6"]
# ///
"""Generate C++ settings declarations and JSON codecs from the proto3 schema.

Run from anywhere: uv run tools/serein/codegen/generate.py [--check]
"""

import argparse
import json
import subprocess
import sys
import tempfile
from pathlib import Path

import jinja2
from codec_model import build_files
from model import SchemaError, build_pages, check_titles

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[2]
PROTO = ROOT / "proto"
OUTPUT = ROOT / "Telegram/SourceFiles/serein"
SCHEMA = "schema/gen"
ROWS = "settings/gen"
HOOKS = "hooks/gen"
SOURCES = f"{SCHEMA}/sources.cmake"
STRINGS = (
    ROOT / "Telegram/Resources/langs/lang.strings",
    ROOT / "Telegram/Resources/langs/serein/serein.strings",
)


def build_image():
    with tempfile.TemporaryDirectory() as temp:
        image = Path(temp) / "image.json"
        subprocess.run(["buf", "build", str(PROTO), "-o", str(image)], check=True, cwd=ROOT)
        return json.loads(image.read_text(encoding="utf-8"))


def cmake_list(name, sources):
    return f"set({name}\n" + "".join(f"    {source}\n" for source in sorted(sources)) + ")\n"


def string_keys():
    keys = set()
    for path in STRINGS:
        for line in path.read_text(encoding="utf-8").splitlines():
            if line.startswith('"lng_'):
                keys.add(line[1 : line.index('"', 1)].split("#")[0])
    return keys


def render(image, known_strings=None):
    environment = jinja2.Environment(
        loader=jinja2.FileSystemLoader(HERE / "templates"),
        trim_blocks=True,
        lstrip_blocks=True,
        keep_trailing_newline=True,
        undefined=jinja2.StrictUndefined,
    )
    settings = environment.get_template("settings.h.j2")
    header = environment.get_template("codec.h.j2")
    implementation = environment.get_template("codec.cpp.j2")
    rows = environment.get_template("settings_rows.h.j2")
    pages = build_pages(image)
    check_titles(pages, string_keys() if known_strings is None else known_strings)
    outputs = {f"{SCHEMA}/{page.header}": settings.render(page=page) for page in pages}
    hook_header = environment.get_template("hooks.h.j2")
    hook_source = environment.get_template("hooks.cpp.j2")
    sources = []
    hooks = []
    for page in pages:
        if page.layout:
            outputs[f"{ROWS}/{page.rows_header}"] = rows.render(page=page)
        outputs[f"{HOOKS}/{page.stem}.h"] = hook_header.render(page=page)
        outputs[f"{HOOKS}/{page.stem}.cpp"] = hook_source.render(page=page)
        hooks.append(f"serein/{HOOKS}/{page.stem}.cpp")
    for file in build_files(image):
        outputs[f"{SCHEMA}/{file.header}"] = header.render(file=file)
        outputs[f"{SCHEMA}/{file.implementation}"] = implementation.render(file=file)
        sources.append(f"serein/{SCHEMA}/{file.implementation}")
    outputs[SOURCES] = cmake_list("serein_generated_sources", sources) + cmake_list(
        "serein_generated_hook_sources", hooks
    )
    return outputs


def existing_files():
    return {
        path.relative_to(OUTPUT).as_posix()
        for folder in (SCHEMA, ROWS, HOOKS)
        if (OUTPUT / folder).exists()
        for path in (OUTPUT / folder).rglob("*")
        if path.is_file()
    }


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument(
        "--check", action="store_true", help="fail if the committed output is stale"
    )
    parser.add_argument("--image", help="use a prebuilt Buf JSON image")
    args = parser.parse_args(argv)
    try:
        image = (
            json.loads(Path(args.image).read_text(encoding="utf-8"))
            if args.image
            else build_image()
        )
        outputs = render(image)
    except (SchemaError, subprocess.CalledProcessError, OSError) as error:
        print(f"generation failed: {error}", file=sys.stderr)
        return 2
    stale = sorted(existing_files() - set(outputs))
    changed = sorted(
        name
        for name, text in outputs.items()
        if not (OUTPUT / name).exists() or (OUTPUT / name).read_text(encoding="utf-8") != text
    )
    if args.check:
        for name in changed + stale:
            print(f"out of date: {OUTPUT.relative_to(ROOT) / name}")
        if changed or stale:
            print("Run: uv run tools/serein/codegen/generate.py")
            return 1
        print(f"{len(outputs)} generated files are up to date.")
        return 0
    for name in changed:
        path = OUTPUT / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(outputs[name], encoding="utf-8")
    for name in stale:
        (OUTPUT / name).unlink()
    print(f"Wrote {len(changed)} and removed {len(stale)} of {len(outputs)} files.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
