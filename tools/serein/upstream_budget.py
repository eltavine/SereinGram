#!/usr/bin/env python3
"""Report and cap how much SereinGram changes upstream Telegram Desktop files.

Every metric is compared with the recorded upstream base commit. Budgets
only ratchet down: lower them in the policy as hooks are consolidated.
"""

import argparse
import json
import re
import subprocess
import sys
from pathlib import Path

from check_file_size import PolicyError, is_owned
from check_file_size import load_policy as load_owned_policy

HERE = Path(__file__).resolve().parent
DEFAULT_POLICY = HERE / "policy" / "upstream.json"
DEFAULT_OWNED_POLICY = HERE / "policy" / "file_size.json"
POLICY_KEYS = {
    "schema_version",
    "upstream",
    "base",
    "source_root",
    "owned_extra",
    "own_include_prefixes",
    "hook_include_prefixes",
    "submodule_overrides",
    "budget",
}
BUDGET_KEYS = (
    "all_files",
    "all_added_lines",
    "source_files",
    "source_added_lines",
    "direct_include_files",
)
INCLUDE = re.compile(r'^\s*#\s*include\s+"([^"]+)"', re.MULTILINE)


def load_policy(path):
    try:
        policy = json.loads(Path(path).read_text(encoding="utf-8"))
    except (OSError, ValueError) as error:
        raise PolicyError(f"cannot read policy {path}: {error}") from error
    if not isinstance(policy, dict) or set(policy) != POLICY_KEYS:
        raise PolicyError(f"policy {path} must have exactly {sorted(POLICY_KEYS)}")
    if policy["schema_version"] != 1:
        raise PolicyError(f"unsupported policy schema {policy['schema_version']}")
    if not re.fullmatch(r"[0-9a-f]{40}", str(policy["base"])):
        raise PolicyError("base must be a full 40-character commit hash")
    budget = policy["budget"]
    if not isinstance(budget, dict) or set(budget) != set(BUDGET_KEYS):
        raise PolicyError(f"budget must have exactly {list(BUDGET_KEYS)}")
    for key in BUDGET_KEYS:
        value = budget[key]
        if not isinstance(value, int) or isinstance(value, bool) or value < 0:
            raise PolicyError(f"budget.{key} must be a non-negative integer")
    for key in (
        "owned_extra",
        "own_include_prefixes",
        "hook_include_prefixes",
        "submodule_overrides",
    ):
        values = policy[key]
        if not isinstance(values, list) or not all(
            isinstance(value, str) and value for value in values
        ):
            raise PolicyError(f"{key} must be a list of non-empty strings")
    return policy


def changed_files(root, base):
    output = subprocess.run(
        ["git", "diff", "--numstat", "--no-renames", "-z", base, "--"],
        cwd=root,
        capture_output=True,
        check=True,
    ).stdout.decode("utf-8")
    for record in filter(None, output.split("\0")):
        added, _deleted, path = record.split("\t", 2)
        yield path, 0 if added == "-" else int(added)


def gitlinks(output, sha_field):
    result = {}
    for line in filter(None, output.split("\n")):
        meta, path = line.split("\t", 1)
        fields = meta.split()
        if fields[0] == "160000":
            result[path] = fields[sha_field]
    return result


def submodule_mismatches(root, policy):
    def run(*args):
        return subprocess.run(
            ["git", *args],
            cwd=root,
            capture_output=True,
            check=True,
        ).stdout.decode("utf-8")

    upstream = gitlinks(run("ls-tree", "-r", policy["base"]), 2)
    staged = gitlinks(run("ls-files", "-s"), 1)
    return sorted(
        (path, staged[path], sha)
        for path, sha in upstream.items()
        if path in staged and staged[path] != sha and path not in policy["submodule_overrides"]
    )


def includes_own_header(path, policy):
    try:
        text = path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return False
    for include in INCLUDE.findall(text):
        own = include.startswith(tuple(policy["own_include_prefixes"]))
        hook = include.startswith(tuple(policy["hook_include_prefixes"]))
        if own and not hook:
            return True
    return False


def measure(root, policy, owned):
    metrics = dict.fromkeys(BUDGET_KEYS, 0)
    offenders = []
    patterns = owned + policy["owned_extra"]
    for path, added in changed_files(root, policy["base"]):
        if is_owned(path, patterns):
            continue
        metrics["all_files"] += 1
        metrics["all_added_lines"] += added
        if not path.startswith(policy["source_root"]):
            continue
        metrics["source_files"] += 1
        metrics["source_added_lines"] += added
        if includes_own_header(Path(root) / path, policy):
            metrics["direct_include_files"] += 1
            offenders.append(path)
    return metrics, sorted(offenders)


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--root", default=HERE.parents[1])
    parser.add_argument("--policy", default=DEFAULT_POLICY)
    parser.add_argument("--owned-policy", default=DEFAULT_OWNED_POLICY)
    parser.add_argument(
        "--list", action="store_true", help="list upstream files that include non-hook headers"
    )
    args = parser.parse_args(argv)
    try:
        policy = load_policy(args.policy)
        owned = load_owned_policy(args.owned_policy)["owned"]
        metrics, offenders = measure(args.root, policy, owned)
        mismatches = submodule_mismatches(args.root, policy)
    except PolicyError as error:
        print(error, file=sys.stderr)
        return 2
    except (OSError, ValueError, subprocess.CalledProcessError) as error:
        print(f"cannot compare with upstream base: {error}", file=sys.stderr)
        return 2
    exceeded = []
    for key in BUDGET_KEYS:
        limit = policy["budget"][key]
        marker = "over budget" if metrics[key] > limit else "ok"
        print(f"{key:<22} {metrics[key]:>6} / {limit:<6} {marker}")
        if metrics[key] > limit:
            exceeded.append(key)
    if args.list:
        for path in offenders:
            print(f"direct include: {path}")
    for path, current, upstream in mismatches:
        print(
            f"Submodule {path} is staged at {current[:10]} but the upstream "
            f"base has {upstream[:10]}; run 'git submodule update {path}' "
            "or list it in submodule_overrides."
        )
    if exceeded:
        print(f"Upstream intrusion over budget: {', '.join(exceeded)}.")
    return 1 if (exceeded or mismatches) else 0


if __name__ == "__main__":
    sys.exit(main())
