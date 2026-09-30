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
warnings="${SEREIN_WARNINGS_BUILD:-out/serein-core-warnings}"
if [ ! -f "$warnings/CMakeCache.txt" ]; then
	cmake -S tools/serein/core_tests -B "$warnings" -G Ninja \
		-DCMAKE_CXX_FLAGS="-Wall -Wextra -Wno-unused-parameter -Wno-switch -Wno-missing-field-initializers -Wno-sign-compare -Wno-deprecated -Wno-deprecated-this-capture -Wrange-loop-construct"
fi
if cmake --build "$warnings" 2>&1 | grep -E 'SourceFiles/serein/.*warning:'; then
	echo "Serein code has warnings that fail the Linux build." >&2
	exit 1
fi
(cd proto && buf lint)
bash tools/serein/proto_breaking.sh "$baseline"
uv run --quiet tools/serein/codegen/generate.py --check
uv run --quiet --with jinja2==3.1.6 python -m unittest discover -s tools/serein/tests
python3 tools/serein/check_file_size.py
python3 tools/serein/check_boundaries.py
python3 tools/serein/check_hook_namespaces.py
python3 tools/serein/upstream_budget.py
python3 tools/serein/check_features.py
if command -v actionlint >/dev/null; then
	actionlint -shellcheck= .github/workflows/serein-*.yml
else
	echo "actionlint is not installed; skipping the workflow lint." >&2
fi
echo "All local SereinGram checks passed."
