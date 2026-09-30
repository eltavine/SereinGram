#!/usr/bin/env python3
"""Merge a Telegram Desktop ref into SereinGram and move the budget baseline.

Run from the repository root on a clean worktree:
    python3 tools/serein/upstream_sync.py v6.3.0

A clean merge is committed on a sync/<ref> branch with the upstream budget
baseline moved to the merged ref. On conflicts the merge is left in progress
and the conflicted files are grouped by who owns them.
"""

import argparse
import json
import re
import subprocess
import sys
from pathlib import Path

import upstream_budget
from check_file_size import PolicyError, is_owned, load_policy as load_owned_policy

HERE = Path(__file__).resolve().parent
REMOTE = "upstream"


class SyncError(Exception):
    pass


def git(root, *args, check=True, stdin=None):
    result = subprocess.run(["git", *args], cwd=root, capture_output=True,
                            text=True, input=stdin)
    if check and result.returncode != 0:
        raise SyncError(f"git {' '.join(args)} failed: {result.stderr.strip()}")
    return result


def branch_name(ref):
    return "sync/" + re.sub(r"[^A-Za-z0-9._-]+", "-", ref).strip("-")


def classify(conflicts, owned, hooked):
    groups = {"serein": [], "hooks": [], "upstream": []}
    for path in conflicts:
        if is_owned(path, owned):
            groups["serein"].append(path)
        elif path in hooked:
            groups["hooks"].append(path)
        else:
            groups["upstream"].append(path)
    return groups


def commit_message(ref, old, new, metrics):
    lines = [
        f"chore(upstream): merge Telegram Desktop {ref}",
        "",
        f"Merge the upstream {ref} ref into SereinGram and move the upstream",
        "budget baseline to it, so the intrusion metrics keep measuring only",
        "the changes SereinGram makes to Telegram Desktop.",
        "",
        f"- Upstream baseline: {old[:12]} -> {new[:12]}.",
        "- Budget measured after the merge:",
    ]
    lines += [f"  - {key}: {value}" for key, value in metrics.items()]
    return "\n".join(lines) + "\n"


def sync(root, ref, policy_path, owned_policy_path, url=None):
    root = Path(root)
    policy_path = Path(policy_path)
    if git(root, "status", "--porcelain").stdout.strip():
        raise SyncError("the worktree has uncommitted changes")
    policy = upstream_budget.load_policy(policy_path)
    owned = load_owned_policy(owned_policy_path)["owned"]
    if REMOTE not in git(root, "remote").stdout.split():
        git(root, "remote", "add", REMOTE, url or policy["upstream"].split("#")[0])
    git(root, "fetch", "--no-tags", REMOTE, ref)
    new = git(root, "rev-parse", "FETCH_HEAD").stdout.strip()
    old = policy["base"]
    if git(root, "merge-base", "--is-ancestor", old, new, check=False).returncode:
        raise SyncError(f"{ref} does not contain the current baseline {old[:12]}")
    patterns = owned + policy["owned_extra"]
    hooked = {path for path, _added in upstream_budget.changed_files(root, old)
              if not is_owned(path, patterns)}
    git(root, "switch", "-q", "-c", branch_name(ref))
    merge = git(root, "merge", "--no-ff", "--no-commit", new, check=False)
    conflicts = git(root, "diff", "--name-only", "--diff-filter=U").stdout.split()
    if merge.returncode or conflicts:
        return classify(conflicts, patterns, hooked), None
    policy["base"] = new
    policy_path.write_text(json.dumps(policy, indent=2, ensure_ascii=False) + "\n",
                           encoding="utf-8")
    metrics, _offenders = upstream_budget.measure(root, policy, owned)
    git(root, "add", str(policy_path.resolve().relative_to(root.resolve())))
    git(root, "commit", "-q", "-F", "-", stdin=commit_message(ref, old, new, metrics))
    return None, metrics


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("ref", help="upstream tag or branch to merge")
    parser.add_argument("--root", default=HERE.parents[1])
    parser.add_argument("--url", help="upstream repository, defaults to the policy")
    parser.add_argument("--policy", default=upstream_budget.DEFAULT_POLICY)
    parser.add_argument("--owned-policy", default=upstream_budget.DEFAULT_OWNED_POLICY)
    args = parser.parse_args(argv)
    try:
        conflicts, metrics = sync(args.root, args.ref, args.policy,
                                  args.owned_policy, args.url)
    except (SyncError, PolicyError, OSError, ValueError,
            subprocess.CalledProcessError) as error:
        print(f"upstream sync failed: {error}", file=sys.stderr)
        return 2
    if conflicts is not None:
        print("Merge stopped on conflicts; resolve them, then commit the merge.")
        for group, paths in conflicts.items():
            for path in paths:
                print(f"{group:<9} {path}")
        return 1
    print(f"Merged {args.ref} on {branch_name(args.ref)}.")
    for key, value in metrics.items():
        print(f"{key:<22} {value}")
    print("Run the guards and push the branch so the platform CI can verify it.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
