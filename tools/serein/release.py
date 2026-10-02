"""Build the notes, checksums and manifest of a SereinGram release.

The asset names come from policy/release_assets.json. Published names never
change, so scripts, package managers and updaters can depend on them; new
assets may be added, existing ones are not renamed or removed.
"""

import argparse
import hashlib
import json
import re
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
POLICY = HERE / "policy" / "release_assets.json"
POLICY_PATH = "tools/serein/policy/release_assets.json"
CHECKSUMS = "SHA256SUMS"
MANIFEST = "release.json"
UPSTREAM = "https://github.com/telegramdesktop/tdesktop"
NAME = re.compile(
    r"^SereinGram-(?P<os>windows|macos|linux)"
    r"-(?P<arch>x86_64|arm64|universal)"
    r"(?:-[a-z0-9]+)?\.(?:exe|zip|dmg|tar\.xz|AppImage|deb|rpm|flatpak)$"
)
HEADER = re.compile(
    r"^(?P<type>[a-z]+)(?:\((?P<scope>[^()]+)\))?(?P<bang>!)?: "
    r"(?P<description>.+)$"
)
MERGE = re.compile(r"^merge Telegram Desktop (?P<ref>\S+)$")
BASELINE = re.compile(r"Upstream baseline: ([0-9a-f]+) -> ([0-9a-f]+)")
SECTIONS = (
    ("breaking", "Breaking changes"),
    ("feat", "Features"),
    ("fix", "Fixes"),
    ("perf", "Performance"),
)
SYSTEMS = {"windows": "Windows", "macos": "macOS", "linux": "Linux"}
KINDS = {
    "installer": "Installer",
    "portable": "Portable archive",
    "disk-image": "Disk image",
    "appimage": "AppImage",
    "deb": "Debian package",
    "rpm": "RPM package",
    "flatpak": "Flatpak bundle",
}


class ReleaseError(Exception):
    pass


def load_assets(path=POLICY):
    data = json.loads(Path(path).read_text(encoding="utf-8"))
    if data.get("schema_version") != 1:
        raise ReleaseError(f"{path}: unsupported schema_version")
    assets = data["assets"]
    names = [asset["name"] for asset in assets]
    if len(set(names)) != len(names):
        raise ReleaseError(f"{path}: duplicate asset names")
    for asset in assets:
        match = NAME.match(asset["name"])
        if not match:
            raise ReleaseError(f"{asset['name']}: breaks the naming rule")
        if (match["os"], match["arch"]) != (asset["os"], asset["arch"]):
            raise ReleaseError(f"{asset['name']}: os or arch disagrees")
        if asset["kind"] not in KINDS:
            raise ReleaseError(f"{asset['name']}: unknown kind")
    return assets


def verify(directory, assets):
    present = {path.name for path in Path(directory).iterdir() if path.is_file()}
    expected = {asset["name"] for asset in assets}
    return sorted(expected - present), sorted(present - expected)


def compatibility(old_assets, new_assets):
    current = {asset["name"]: asset for asset in new_assets}
    problems = []
    for asset in old_assets:
        if asset["name"] not in current:
            problems.append(f"{asset['name']}: a published asset cannot be removed or renamed")
        elif current[asset["name"]] != asset:
            problems.append(f"{asset['name']}: the os, arch and kind of an asset cannot change")
    return problems


def baseline_assets(root, ref):
    result = subprocess.run(
        ["git", "show", f"{ref}:{POLICY_PATH}"],
        cwd=root,
        capture_output=True,
        text=True,
        check=False,
    )
    if result.returncode != 0:
        return None
    return json.loads(result.stdout)["assets"]


def sha256(path):
    digest = hashlib.sha256()
    with open(path, "rb") as file:
        for chunk in iter(lambda: file.read(1 << 20), b""):
            digest.update(chunk)
    return digest.hexdigest()


def checksums(directory, assets):
    names = sorted(asset["name"] for asset in assets)
    return "".join(f"{sha256(Path(directory) / name)}  {name}\n" for name in names)


def download_url(repo, tag, name):
    return f"https://github.com/{repo}/releases/download/{tag}/{name}"


def manifest(directory, assets, info):
    entries = []
    for asset in assets:
        path = Path(directory) / asset["name"]
        entries.append(
            {
                "name": asset["name"],
                "os": asset["os"],
                "arch": asset["arch"],
                "kind": asset["kind"],
                "size": path.stat().st_size,
                "sha256": sha256(path),
                "url": download_url(info["repo"], info["tag"], asset["name"]),
            }
        )
    document = {
        "schema_version": 1,
        "channel": info["channel"],
        "tag": info["tag"],
        "version": info["version"],
        "commit": info["commit"],
        "date": info["date"],
        "assets": entries,
    }
    return json.dumps(document, indent=2) + "\n"


def read_version(root=ROOT):
    text = (Path(root) / "Telegram" / "build" / "version").read_text()
    match = re.search(r"^AppVersionStr\s+(\S+)$", text, re.MULTILINE)
    if not match:
        raise ReleaseError("Telegram/build/version has no AppVersionStr")
    return match[1]


def first_parent_log(root, previous, commit, limit):
    revision = f"{previous}..{commit}" if previous else commit
    command = ["git", "log", "--first-parent", "--format=%H%x1f%s%x1f%b%x1e", revision]
    if not previous:
        command.insert(3, f"--max-count={limit}")
    output = subprocess.run(command, cwd=root, check=True, capture_output=True, text=True).stdout
    commits = []
    for raw in output.split("\x1e"):
        record = raw.strip("\n")
        if record:
            sha, subject, body = record.split("\x1f", 2)
            commits.append((sha, subject, body))
    return commits


def classify(commits):
    groups = {key: [] for key in ("breaking", "feat", "fix", "perf", "upstream", "other")}
    for sha, subject, body in commits:
        match = HEADER.match(subject)
        merge = match and match["scope"] == "upstream" and MERGE.match(match["description"])
        if merge:
            baseline = BASELINE.search(body)
            groups["upstream"].append((sha, merge["ref"], baseline[2] if baseline else None))
            continue
        if not match:
            groups["other"].append((sha, None, subject))
            continue
        entry = (sha, match["scope"], match["description"])
        if match["bang"] or "BREAKING CHANGE" in body:
            groups["breaking"].append(entry)
        elif match["type"] in ("feat", "fix", "perf"):
            groups[match["type"]].append(entry)
        else:
            groups["other"].append(entry)
    return groups


def commit_link(repo, sha):
    return f"[`{sha[:7]}`](https://github.com/{repo}/commit/{sha})"


def entry_line(repo, entry):
    sha, scope, description = entry
    text = description[:1].upper() + description[1:]
    prefix = f"**{scope}:** " if scope else ""
    return f"- {prefix}{text} ({commit_link(repo, sha)})"


def change_lines(repo, groups):
    lines = []
    for key, title in SECTIONS:
        if groups[key]:
            lines += ["", f"### {title}", ""]
            lines += [entry_line(repo, entry) for entry in groups[key]]
    if groups["upstream"]:
        lines += ["", "### Telegram Desktop", ""]
        for sha, ref, baseline in groups["upstream"]:
            target = f" up to [`{baseline[:10]}`]({UPSTREAM}/commit/{baseline})" if baseline else ""
            lines.append(f"- Merged Telegram Desktop `{ref}`{target} ({commit_link(repo, sha)})")
    if groups["other"]:
        count = len(groups["other"])
        lines += [
            "",
            "<details>",
            f"<summary>{count} other changes (build, tooling, documentation)</summary>",
            "",
        ]
        lines += [entry_line(repo, entry) for entry in groups["other"]]
        lines += ["", "</details>"]
    return lines


def download_lines(repo, tag, assets):
    lines = [
        "",
        "## Downloads",
        "",
        "| System | Architecture | Package | File |",
        "| --- | --- | --- | --- |",
    ]
    for asset in assets:
        url = download_url(repo, tag, asset["name"])
        lines.append(
            f"| {SYSTEMS[asset['os']]} | {asset['arch']} "
            f"| {KINDS[asset['kind']]} | [{asset['name']}]({url}) |"
        )
    return lines


def verify_lines():
    return [
        "",
        "## Verify",
        "",
        f"`{CHECKSUMS}` lists the SHA-256 of every file above, and "
        f"`{MANIFEST}` carries the same data for scripts.",
        "",
        "```sh",
        f"sha256sum --ignore-missing -c {CHECKSUMS}       # Linux",
        f"shasum -a 256 --ignore-missing -c {CHECKSUMS}   # macOS",
        "```",
        "",
        f"On Windows, compare `(Get-FileHash <file>).Hash` with the line in `{CHECKSUMS}`.",
        "",
        "These builds are not signed with a developer certificate: on macOS "
        "open the app with Control-click and Open the first time, on "
        "Windows confirm the SmartScreen prompt with More info and Run "
        "anyway.",
    ]


def notes(commits, assets, info):
    label = "Nightly" if info["channel"] == "nightly" else info["tag"]
    commit = info["commit"]
    lines = [
        f"**SereinGram {label}**, built on {info['date']} from "
        f"{commit_link(info['repo'], commit)}, based on Telegram Desktop "
        f"{info['version']}.",
        "",
    ]
    previous = info.get("previous")
    since = info.get("since") or "the previous build"
    lines.append(f"## Changes since {since}")
    lines.append("")
    if previous:
        lines.append(
            f"[`{previous[:7]}...{commit[:7]}`]"
            f"(https://github.com/{info['repo']}/compare/"
            f"{previous}...{commit}), {len(commits)} commits."
        )
    else:
        lines.append(f"First build of this channel, latest {len(commits)} commits.")
    if commits:
        lines += change_lines(info["repo"], classify(commits))
    else:
        lines += ["", "No changes."]
    lines += download_lines(info["repo"], info["tag"], assets)
    lines += verify_lines()
    return "\n".join(lines) + "\n"


def check_compatibility(root, ref, assets):
    old = None if re.fullmatch(r"0+", ref) else baseline_assets(root, ref)
    if old is None:
        print(f"No release asset list at {ref}; nothing to compare.")
        return 0
    problems = compatibility(old, assets)
    for problem in problems:
        print(problem, file=sys.stderr)
    if problems:
        return 1
    print(f"The {len(old)} release assets of {ref} are all kept.")
    return 0


def write(text, output):
    if output:
        Path(output).write_text(text, encoding="utf-8", newline="\n")
    else:
        sys.stdout.write(text)


def add_info_arguments(parser, previous=False):
    parser.add_argument("--repo", required=True)
    parser.add_argument("--tag", required=True)
    parser.add_argument("--channel", required=True, choices=("nightly", "release"))
    parser.add_argument("--commit", required=True)
    parser.add_argument("--date", required=True)
    parser.add_argument("--version")
    if previous:
        parser.add_argument("--previous", default="")
        parser.add_argument("--since")
        parser.add_argument("--limit", type=int, default=50)
        parser.add_argument("--root", default=str(ROOT))


def info_from(args):
    keys = ("repo", "tag", "channel", "commit", "date", "previous", "since")
    info = {key: getattr(args, key, None) for key in keys}
    info["version"] = args.version or read_version()
    return info


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--assets", default=str(POLICY))
    commands = parser.add_subparsers(dest="command", required=True)
    for name in ("verify", "checksums", "manifest"):
        command = commands.add_parser(name)
        command.add_argument("directory")
        command.add_argument("-o", "--output")
        if name == "manifest":
            add_info_arguments(command)
    command = commands.add_parser("notes")
    command.add_argument("-o", "--output")
    add_info_arguments(command, previous=True)
    command = commands.add_parser("compat")
    command.add_argument("--baseline", required=True)
    command.add_argument("--root", default=str(ROOT))
    args = parser.parse_args(argv)
    try:
        assets = load_assets(args.assets)
        if args.command == "compat":
            return check_compatibility(args.root, args.baseline, assets)
        if args.command == "verify":
            missing, unexpected = verify(args.directory, assets)
            for name in missing:
                print(f"Missing release asset: {name}", file=sys.stderr)
            for name in unexpected:
                print(f"Unexpected release asset: {name}", file=sys.stderr)
            return 1 if missing or unexpected else 0
        if args.command == "checksums":
            write(checksums(args.directory, assets), args.output)
        elif args.command == "manifest":
            write(manifest(args.directory, assets, info_from(args)), args.output)
        else:
            commits = first_parent_log(args.root, args.previous, args.commit, args.limit)
            write(notes(commits, assets, info_from(args)), args.output)
    except (ReleaseError, OSError, subprocess.CalledProcessError) as error:
        print(f"release.py: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
