#!/usr/bin/env bash
# Runs every SereinGram check that does not need the full Telegram build.
set -euo pipefail

cd "$(dirname "$0")/../.."
build="${SEREIN_CORE_BUILD:-out/serein-core-tests}"
baseline="${SEREIN_PROTO_BASELINE:-origin/main}"

if [ ! -f "$build/CMakeCache.txt" ]; then
	cmake -S tools/serein/core_tests -B "$build" -G Ninja
fi
cmake --build "$build"
"$build/test_serein"
(cd proto && buf lint)
bash tools/serein/proto_breaking.sh "$baseline"
uv run --quiet tools/serein/codegen/generate.py --check
uv run --quiet --with jinja2==3.1.6 python -m unittest discover -s tools/serein/tests
python3 tools/serein/check_file_size.py
python3 tools/serein/check_boundaries.py
python3 tools/serein/upstream_budget.py
echo "All local SereinGram checks passed."
