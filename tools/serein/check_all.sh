#!/usr/bin/env bash
# Runs every SereinGram check that does not need the full Telegram build.
set -euo pipefail

cd "$(dirname "$0")/../.."
build="${SEREIN_CORE_BUILD:-out/serein-core-tests}"
baseline="${SEREIN_PROTO_BASELINE:-origin/main}"

have() {
	if command -v "$1" >/dev/null; then
		return 0
	fi
	echo "$1 is not installed; skipping $2 (CI runs it)." >&2
	return 1
}

if [ ! -f "$build/compile_commands.json" ]; then
	cmake -S tools/serein/core_tests -B "$build" -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
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
(cd proto && buf lint && buf format --diff --exit-code)
bash tools/serein/proto_breaking.sh "$baseline"
python3 tools/serein/release.py compat --baseline "$baseline"
uv run --quiet tools/serein/codegen/generate.py --check
uv run --quiet --with jinja2==3.1.6 python -m unittest discover -s tools/serein/tests
python3 tools/serein/check_style.py
uvx --quiet ruff@0.16.10 format --check tools/serein
uvx --quiet ruff@0.16.10 check --quiet tools/serein
if have shellcheck "the shell script lint"; then
	git ls-files -z 'tools/serein/*.sh' 'packaging/**/*.sh' tools/serein/githooks/commit-msg \
		| xargs -0 shellcheck
fi
git ls-files -z '.github/workflows/serein-*.yml' 'packaging/**/*.yml' 'packaging/**/*.yaml' \
	crowdin.yml tools/serein/yamllint.yml \
	| xargs -0 uvx --quiet yamllint@1.37.1 --strict -c tools/serein/yamllint.yml
if have npx "the documentation lint"; then
	npx --yes markdownlint-cli2@0.19.1 --config tools/serein/serein.markdownlint-cli2.jsonc \
		'docs/serein/**/*.md' BRANDING.md README.md >/dev/null
fi
python3 tools/serein/check_file_size.py
python3 tools/serein/check_boundaries.py
python3 tools/serein/check_hook_namespaces.py
python3 tools/serein/check_includes.py
python3 tools/serein/check_sources.py
python3 tools/serein/check_packaging.py
python3 tools/serein/upstream_budget.py
python3 tools/serein/check_features.py
if have actionlint "the workflow lint"; then
	actionlint .github/workflows/serein-*.yml
	uvx --quiet zizmor@1.16.3 --offline --quiet .github/workflows/serein-*.yml
fi
if [ "${SEREIN_SKIP_TIDY:-0}" != 1 ]; then
	python3 tools/serein/run_clang_tidy.py -p "$build" \
		--clang-tidy "uvx --quiet --from clang-tidy==21.1.1 clang-tidy"
fi
echo "All local SereinGram checks passed."
