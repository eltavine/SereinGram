#!/usr/bin/env python3
"""Check that distribution packaging pins the same dependency revisions
as the upstream snap recipe, so an upstream sync cannot leave the Arch
or Flatpak builds on stale commits."""

import argparse
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SNAP = "snap/snapcraft.yaml"
PKGBUILD = "packaging/arch/PKGBUILD"
FLATPAK = "packaging/flatpak/io.github.eltavine.SereinGram.yml"
FLATPAK_SOURCES = {
    "tde2e": "https://github.com/tdlib/td.git",
    "webrtc": "https://github.com/desktop-app/tg_owt.git",
    "tlottie": "https://github.com/dkaraush/tlottie.git",
    "patches": "https://github.com/desktop-app/patches.git",
}
PKGBUILD_COMMIT = re.compile(r"^_td_commit=([0-9a-f]{40})$", re.M)
QT_ARCHIVE = re.compile(r"/qt-everywhere-src-([0-9.]+)\.tar\.xz$", re.M)


def snap_value(text, part, key):
    block = re.search(rf"^  {re.escape(part)}:\n((?:    .*\n|\n)*)", text, re.M)
    if not block:
        return None
    value = re.search(rf"^    {key}: (\S+)$", block.group(1), re.M)
    return value.group(1) if value else None


def flatpak_commits(text, url):
    return re.findall(rf"url: {re.escape(url)}\n\s+commit: ([0-9a-f]{{40}})", text)


def problems(root):
    root = Path(root)
    snap = (root / SNAP).read_text(encoding="utf-8")
    pkgbuild = (root / PKGBUILD).read_text(encoding="utf-8")
    flatpak = (root / FLATPAK).read_text(encoding="utf-8")
    result = []
    for part, url in FLATPAK_SOURCES.items():
        expected = snap_value(snap, part, "source-commit")
        if not expected or not re.fullmatch(r"[0-9a-f]{40}", expected):
            result.append(f"{SNAP} has no 40-character {part} source-commit.")
            continue
        found = flatpak_commits(flatpak, url)
        if not found:
            result.append(f"{FLATPAK} does not build {url} at a commit.")
        for commit in found:
            if commit != expected:
                result.append(
                    f"{FLATPAK} pins {url} at {commit[:10]}, but {SNAP} "
                    f"uses {expected[:10]} for {part}."
                )
        if part == "tde2e":
            match = PKGBUILD_COMMIT.search(pkgbuild)
            if not match:
                result.append(f"{PKGBUILD} has no 40-character _td_commit.")
            elif match.group(1) != expected:
                result.append(
                    f"{PKGBUILD} pins tdlib {match.group(1)[:10]}, but "
                    f"{SNAP} builds tde2e from {expected[:10]}; update "
                    "_td_commit."
                )
    tag = snap_value(snap, "qt", "source-tag")
    archive = QT_ARCHIVE.search(flatpak)
    if not tag or not archive:
        result.append(f"Cannot compare the Qt version of {SNAP} and {FLATPAK}.")
    elif tag.removeprefix("v") != archive.group(1):
        result.append(f"{FLATPAK} builds Qt {archive.group(1)}, but {SNAP} uses {tag}.")
    return result


def update(root):
    """Move the Arch and Flatpak commit pins to the snap recipe."""
    root = Path(root)
    if not all((root / path).exists() for path in (SNAP, PKGBUILD, FLATPAK)):
        return []
    snap = (root / SNAP).read_text(encoding="utf-8")
    texts = {
        PKGBUILD: (root / PKGBUILD).read_text(encoding="utf-8"),
        FLATPAK: (root / FLATPAK).read_text(encoding="utf-8"),
    }
    changed = []
    for part, url in FLATPAK_SOURCES.items():
        expected = snap_value(snap, part, "source-commit")
        if not expected or not re.fullmatch(r"[0-9a-f]{40}", expected):
            continue
        source = re.compile(rf"(url: {re.escape(url)}\n\s+commit: )([0-9a-f]{{40}})")
        stale = {match.group(2) for match in source.finditer(texts[FLATPAK])}
        if part == "tde2e":
            stale |= set(PKGBUILD_COMMIT.findall(texts[PKGBUILD]))
            texts[PKGBUILD] = PKGBUILD_COMMIT.sub(f"_td_commit={expected}", texts[PKGBUILD])
        texts[FLATPAK] = source.sub(
            lambda match, expected=expected: match.group(1) + expected, texts[FLATPAK]
        )
        for commit in sorted(stale - {expected}):
            changed.append(f"{part} {commit[:10]} -> {expected[:10]}")
    for path, text in texts.items():
        (root / path).write_text(text, encoding="utf-8")
    return changed


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=ROOT)
    parser.add_argument(
        "--update", action="store_true", help="rewrite the commit pins from the snap recipe first"
    )
    args = parser.parse_args(argv)
    if args.update:
        for change in update(args.root):
            print(f"Updated {change}.")
            if change.startswith("tlottie "):
                print(
                    "Regenerate packaging/flatpak/tlottie-cargo-sources.yml "
                    "from the new tlottie Cargo.lock."
                )
    found = problems(args.root)
    for problem in found:
        print(problem)
    if found:
        return 1
    print("Packaging pins the upstream dependency revisions.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
