#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
baseline_ref="${1:-}"

if [[ -z "$baseline_ref" ]]; then
  printf 'usage: %s <git-baseline-ref>\n' "${BASH_SOURCE[0]}" >&2
  exit 2
fi

# A zero before-SHA is GitHub's representation of a new branch.
if [[ "$baseline_ref" =~ ^0+$ ]]; then
  printf 'No baseline commit; skipping the schema compatibility check.\n'
  exit 0
fi

if ! git -C "$repo_root" cat-file -e "${baseline_ref}^{commit}" 2>/dev/null; then
  printf 'Baseline ref does not resolve to a commit: %s\n' "$baseline_ref" >&2
  exit 2
fi

if ! git -C "$repo_root" cat-file -e "${baseline_ref}:proto/buf.yaml" 2>/dev/null; then
  printf 'No schema module at %s; treating this as the bootstrap commit.\n' "$baseline_ref"
  exit 0
fi

cd "$repo_root"
exec buf breaking proto --against ".git#ref=${baseline_ref},subdir=proto"
