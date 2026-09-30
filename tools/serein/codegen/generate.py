#!/usr/bin/env python3
# /// script
# requires-python = ">=3.11"
# dependencies = ["jinja2==3.1.6"]
# ///
"""Generate C++ settings declarations from the proto3 schema.

Run from anywhere: uv run tools/serein/codegen/generate.py [--check]
"""

import argparse
import json
import subprocess
import sys
import tempfile
from pathlib import Path

import jinja2

from model import SchemaError, build_pages

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[2]
PROTO = ROOT / "proto"
OUTPUT = ROOT / "Telegram/SourceFiles/serein/schema/gen"


def build_image():
    with tempfile.TemporaryDirectory() as temp:
        image = Path(temp) / "image.json"
        subprocess.run(["buf", "build", str(PROTO), "-o", str(image)],
                       check=True, cwd=ROOT)
        return json.loads(image.read_text(encoding="utf-8"))


def render(image):
    environment = jinja2.Environment(
        loader=jinja2.FileSystemLoader(HERE / "templates"),
        trim_blocks=True,
        lstrip_blocks=True,
        keep_trailing_newline=True,
        undefined=jinja2.StrictUndefined,
    )
    template = environment.get_template("settings.h.j2")
    return {page.header: template.render(page=page) for page in build_pages(image)}


def existing_files():
    if not OUTPUT.exists():
        return set()
    return {str(path.relative_to(OUTPUT)) for path in OUTPUT.rglob("*.h")}


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--check", action="store_true",
                        help="fail if the committed output is stale")
    parser.add_argument("--image", help="use a prebuilt Buf JSON image")
    args = parser.parse_args(argv)
    try:
        image = (json.loads(Path(args.image).read_text(encoding="utf-8"))
                 if args.image else build_image())
        outputs = render(image)
    except (SchemaError, subprocess.CalledProcessError, OSError) as error:
        print(f"generation failed: {error}", file=sys.stderr)
        return 2
    stale = sorted(existing_files() - set(outputs))
    changed = sorted(name for name, text in outputs.items()
                     if not (OUTPUT / name).exists()
                     or (OUTPUT / name).read_text(encoding="utf-8") != text)
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
