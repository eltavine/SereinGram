#!/usr/bin/env python3
"""Fail when a SereinGram module includes something its layer may not know.

Each owned file belongs to the module with the longest matching prefix.
Quoted includes resolve to another owned module, an upstream application
header (a file under the source root, or an app prefix such as generated
styles) or a desktop-app library header (everything else).
"""

import argparse
import json
import re
import sys
from pathlib import Path

from check_file_size import PolicyError, list_files

HERE = Path(__file__).resolve().parent
DEFAULT_POLICY = HERE / "policy" / "boundaries.json"
POLICY_KEYS = {"schema_version", "source_root", "app_prefixes", "modules"}
MODULE_KEYS = {"own", "app", "libraries"}
INCLUDE = re.compile(r'^\s*#\s*include\s+"([^"]+)"', re.MULTILINE)
SOURCES = (".h", ".hpp", ".cpp", ".mm", ".m")


def load_policy(path):
    try:
        policy = json.loads(Path(path).read_text(encoding="utf-8"))
    except (OSError, ValueError) as error:
        raise PolicyError(f"cannot read policy {path}: {error}") from error
    if not isinstance(policy, dict) or set(policy) != POLICY_KEYS:
        raise PolicyError(f"policy {path} must have exactly {sorted(POLICY_KEYS)}")
    if policy["schema_version"] != 1:
        raise PolicyError(f"unsupported policy schema {policy['schema_version']}")
    modules = policy["modules"]
    if not isinstance(modules, dict) or not modules:
        raise PolicyError("modules must be a non-empty object")
    for name, rules in modules.items():
        if not name.endswith("/") or not isinstance(rules, dict) or set(rules) != MODULE_KEYS:
            raise PolicyError(f"module {name} must end with / and have {sorted(MODULE_KEYS)}")
        if not isinstance(rules["app"], bool) or not isinstance(rules["libraries"], bool):
            raise PolicyError(f"module {name}: app and libraries must be booleans")
        if not isinstance(rules["own"], list) or not all(
            isinstance(item, str) and item for item in rules["own"]
        ):
            raise PolicyError(f"module {name}: own must list non-empty strings")
    return policy


def module_of(path, modules):
    matches = [name for name in modules if path.startswith(name)]
    return max(matches, key=len) if matches else None


def allowed_own(include, own):
    return any(include == item or (item.endswith("/") and include.startswith(item)) for item in own)


def classify(include, root, policy):
    if include.startswith(tuple(policy["modules"])):
        return "own"
    if include.startswith(tuple(policy["app_prefixes"])):
        return "app"
    if (Path(root) / policy["source_root"] / include).is_file():
        return "app"
    return "library"


def find_violations(root, policy):
    source_root = policy["source_root"]
    modules = policy["modules"]
    violations = []
    checked = 0
    for name in list_files(root):
        if not name.startswith(source_root) or not name.endswith(SOURCES):
            continue
        relative = name[len(source_root) :]
        module = module_of(relative, modules)
        full = Path(root) / name
        if module is None or not full.is_file():
            continue
        checked += 1
        rules = modules[module]
        text = full.read_text(encoding="utf-8", errors="replace")
        for include in INCLUDE.findall(text):
            kind = classify(include, root, policy)
            if kind == "own" and not allowed_own(include, rules["own"]):
                violations.append((relative, include, f"{module} may not include it"))
            elif kind == "app" and not rules["app"]:
                violations.append((relative, include, f"{module} may not know upstream code"))
            elif kind == "library" and not rules["libraries"]:
                violations.append((relative, include, f"{module} may not use libraries"))
    return checked, violations


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--root", default=HERE.parents[1])
    parser.add_argument("--policy", default=DEFAULT_POLICY)
    args = parser.parse_args(argv)
    try:
        policy = load_policy(args.policy)
        checked, violations = find_violations(args.root, policy)
    except PolicyError as error:
        print(error, file=sys.stderr)
        return 2
    except OSError as error:
        print(f"cannot read sources: {error}", file=sys.stderr)
        return 2
    for path, include, reason in violations:
        print(f'{path}: includes "{include}": {reason}')
    if violations:
        print(f"{len(violations)} module boundary violation(s).")
        return 1
    print(f"Checked includes of {checked} owned files; boundaries hold.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
