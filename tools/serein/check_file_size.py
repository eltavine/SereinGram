#!/usr/bin/env python3
"""Fail when a SereinGram-owned source file exceeds the line limit.

Upstream Telegram Desktop files are exempt: only paths listed under
"owned" in the policy are checked.
"""

import argparse
import fnmatch
import json
import subprocess
import sys
from pathlib import Path

DEFAULT_POLICY = Path(__file__).resolve().parent / "policy" / "file_size.json"
POLICY_KEYS = {"schema_version", "max_lines", "owned", "extensions", "filenames"}


class PolicyError(Exception):
    pass


def load_policy(path):
    try:
        policy = json.loads(Path(path).read_text(encoding="utf-8"))
    except (OSError, ValueError) as error:
        raise PolicyError(f"cannot read policy {path}: {error}") from error
    if not isinstance(policy, dict) or set(policy) != POLICY_KEYS:
        raise PolicyError(f"policy {path} must have exactly {sorted(POLICY_KEYS)}")
    if policy["schema_version"] != 1:
        raise PolicyError(f"unsupported policy schema {policy['schema_version']}")
    limit = policy["max_lines"]
    if not isinstance(limit, int) or isinstance(limit, bool) or limit <= 0:
        raise PolicyError("max_lines must be a positive integer")
    for key in ("owned", "extensions", "filenames"):
        values = policy[key]
        if not isinstance(values, list) or not all(
                isinstance(value, str) and value for value in values):
            raise PolicyError(f"{key} must be a list of non-empty strings")
    if not policy["owned"]:
        raise PolicyError("owned must not be empty")
    return policy


def list_files(root):
    result = subprocess.run(
        ["git", "ls-files", "-z", "--cached", "--others", "--exclude-standard"],
        cwd=root, capture_output=True, check=True)
    names = result.stdout.decode("utf-8").split("\0")
    return sorted({name for name in names if name})


def is_owned(path, patterns):
    for pattern in patterns:
        if pattern.endswith("/"):
            if path.startswith(pattern):
                return True
        elif fnmatch.fnmatchcase(path, pattern):
            return True
    return False


def is_source(path, policy):
    name = Path(path).name
    return name in policy["filenames"] or Path(path).suffix in policy["extensions"]


def count_lines(path):
    data = path.read_bytes()
    if b"\0" in data:
        return None
    return len(data.splitlines())


def find_violations(root, policy):
    checked = []
    violations = []
    for name in list_files(root):
        if not is_owned(name, policy["owned"]) or not is_source(name, policy):
            continue
        full = Path(root) / name
        if not full.is_file():
            continue
        lines = count_lines(full)
        if lines is None:
            continue
        checked.append((lines, name))
        if lines > policy["max_lines"]:
            violations.append((lines, name))
    violations.sort(reverse=True)
    return checked, violations


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--root", default=Path(__file__).resolve().parents[2])
    parser.add_argument("--policy", default=DEFAULT_POLICY)
    args = parser.parse_args(argv)
    try:
        policy = load_policy(args.policy)
        checked, violations = find_violations(args.root, policy)
    except PolicyError as error:
        print(error, file=sys.stderr)
        return 2
    except (OSError, subprocess.CalledProcessError) as error:
        print(f"cannot list repository files: {error}", file=sys.stderr)
        return 2
    limit = policy["max_lines"]
    for lines, name in violations:
        print(f"{name}: {lines} lines (limit {limit})")
    if violations:
        print(f"{len(violations)} owned source file(s) exceed {limit} lines.")
        return 1
    largest = max(checked, default=(0, "none"))
    print(f"Checked {len(checked)} owned source files; "
          f"largest is {largest[1]} with {largest[0]} lines (limit {limit}).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
