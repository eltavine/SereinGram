#!/usr/bin/env bash
# Installs the Linux packages in a container of every supported distribution
# and checks the installed files and the dynamic libraries they need.
# A distribution whose image or package lists stay unreachable in every
# attempt is inconclusive; any other failure fails the test.
#
# Usage: linux_package_test.sh <directory with the packages> <x86_64|arm64>
set -euo pipefail

if [ $# -ne 2 ]; then
	echo "usage: $0 <artifact directory> <x86_64|arm64>" >&2
	exit 2
fi
artifacts=$(cd "$1" && pwd)
arch=$2
attempts=${SEREIN_PACKAGE_TEST_ATTEMPTS:-3}
first_wait=${SEREIN_PACKAGE_TEST_WAIT:-30}
limit=${SEREIN_PACKAGE_TEST_LIMIT:-600}
unreachable=75
broken=70

for suffix in .deb .rpm -portable; do
	if [ ! -f "$artifacts/SereinGram-linux-$arch$suffix" ]; then
		echo "$artifacts has no SereinGram-linux-$arch$suffix." >&2
		exit 2
	fi
done
deb="/artifact/SereinGram-linux-$arch.deb"
rpm="/artifact/SereinGram-linux-$arch.rpm"
portable="/artifact/SereinGram-linux-$arch-portable"

refresh() {
	"$@" || exit "$unreachable"
}

mirror() {
	if [ -n "$name" ] && [ "$enabled" = 1 ] && [ -n "$url" ]; then
		count=$((count + 1))
		zypper --non-interactive --quiet modifyrepo --disable "$name"
		zypper --non-interactive --quiet addrepo --priority 1 "$url" "origin-$count"
	fi
	name="" enabled=0 url=""
}

origin() {
	local file line name enabled url count=0
	[ "$PACKAGE_TEST_ATTEMPT" -gt 1 ] || return 0
	for file in "${SEREIN_ZYPP_REPOS:-/etc/zypp/repos.d}"/*.repo; do
		name="" enabled=0 url=""
		while IFS= read -r line || [ -n "$line" ]; do
			case "$line" in
			"["*"]")
				mirror
				name=${line#[}
				name=${name%]}
				;;
			enabled=1)
				enabled=1
				;;
			baseurl=http://download.opensuse.org/* | baseurl=https://download.opensuse.org/* | \
				baseurl=http://cdn.opensuse.org/* | baseurl=https://cdn.opensuse.org/*)
				if [ -z "$url" ]; then
					url="https://downloadcontent.opensuse.org/${line#baseurl=*://*/}"
				fi
				;;
			esac
		done <"$file"
		mirror
	done
}

prelude="unreachable=$unreachable
$(declare -f refresh mirror origin)"

installed="test -x /usr/bin/SereinGram || exit $broken
test -f /usr/share/applications/io.github.eltavine.SereinGram.desktop || exit $broken
test -f /usr/share/metainfo/io.github.eltavine.SereinGram.metainfo.xml || exit $broken
test -f /usr/share/icons/hicolor/256x256/apps/io.github.eltavine.SereinGram.png || exit $broken
if ldd /usr/bin/SereinGram | grep 'not found'; then exit $broken; fi"

header="### Linux package integration test ($arch)

| Distribution | Image | Result |
| --- | --- | --- |"
passes=0
failures=0
runs=0
rows=""
if [ -n "${GITHUB_STEP_SUMMARY:-}" ]; then
	printf '%s\n' "$header" >>"$GITHUB_STEP_SUMMARY"
fi

row() {
	rows+="$1"$'\n'
	if [ -n "${GITHUB_STEP_SUMMARY:-}" ]; then
		printf '%s\n' "$1" >>"$GITHUB_STEP_SUMMARY"
	fi
}

check() {
	local name=$1 image=$2 script=$3
	local attempt=1 wait=$first_wait status pulled=false failure="" output container result
	echo "::group::$name ($image)"
	while true; do
		status=0
		if [ "$pulled" = false ]; then
			if output=$(timeout "$limit" docker pull --quiet "$image" 2>&1); then
				pulled=true
			else
				echo "$output" >&2
				status=$unreachable
				if grep -Eqi 'manifest unknown|no matching manifest|repository does not exist|pull access denied' <<<"$output"; then
					status=$broken
					failure="the image does not exist"
				fi
			fi
		fi
		if [ "$pulled" = true ]; then
			runs=$((runs + 1))
			container="serein-package-test-$$-$runs"
			timeout --kill-after=60 "$limit" docker run --rm --init --name "$container" \
				-e "PACKAGE_TEST_ATTEMPT=$attempt" -v "$artifacts:/artifact:ro" "$image" \
				bash -euo pipefail -c "$prelude
$script" || status=$?
			if [ "$status" -eq 124 ] || [ "$status" -eq 137 ]; then
				timeout 60 docker rm -f "$container" >/dev/null 2>&1 || true
				status=$unreachable
			elif [ "$status" -ne 0 ] && [ "$status" -ne "$unreachable" ]; then
				failure="installing or checking the package failed with exit code $status in attempt $attempt"
			fi
		fi
		if [ "$status" -eq 0 ] || [ "$status" -eq "$broken" ] || [ "$attempt" -ge "$attempts" ]; then
			break
		fi
		echo "$name: attempt $attempt of $attempts failed with exit code $status; retrying in $wait s." >&2
		sleep "$wait"
		attempt=$((attempt + 1))
		wait=$((wait * 2))
	done
	echo "::endgroup::"
	if [ "$status" -eq 0 ]; then
		passes=$((passes + 1))
		result="passed"
		if [ "$attempt" -gt 1 ]; then
			result="passed in attempt $attempt"
		fi
	elif [ -z "$failure" ]; then
		result="inconclusive, the image or the package repositories were unreachable or timed out"
		echo "::warning title=Package test inconclusive::$name ($arch): the image or the package repositories were unreachable or timed out in $attempt attempts."
	else
		failures=$((failures + 1))
		result="failed, $failure"
		echo "::error title=Package test failed::$name ($arch): $failure."
	fi
	row "| $name | \`$image\` | $result |"
}

apt="export DEBIAN_FRONTEND=noninteractive
refresh apt-get -qq -o Acquire::Retries=3 --error-on=any update
apt-get install -y -qq -o Acquire::Retries=3 --no-install-recommends $deb >/dev/null
$installed"
check "Debian 12" debian:12-slim "$apt"
check "Debian 13" debian:13-slim "$apt"
check "Ubuntu 22.04" ubuntu:22.04 "$apt"
check "Ubuntu 24.04" ubuntu:24.04 "$apt"
check "Ubuntu 26.04" ubuntu:26.04 "$apt"

check "Fedora 43" fedora:43 "refresh dnf makecache -q
dnf install -y -q $rpm
$installed"
zypper="origin
refresh zypper --non-interactive --gpg-auto-import-keys refresh
zypper --non-interactive --quiet --no-refresh install --allow-unsigned-rpm $rpm
$installed"
check "openSUSE Tumbleweed" opensuse/tumbleweed "$zypper"
check "openSUSE Leap 16.1" opensuse/leap:16.1 "$zypper"

# The Linux Mint and Arch Linux images exist for x86_64 only. Arch users run
# the portable build or the AppImage; packaging/arch builds against system
# libraries in its own workflow.
if [ "$arch" = x86_64 ]; then
	check "Linux Mint 22.3" linuxmintd/mint22.3-amd64 "$apt"
	check "Arch Linux" archlinux:latest "refresh pacman -Sy --noconfirm
pacman -S --noconfirm --needed cairo fontconfig freetype2 glib2 pango >/dev/null
install -Dm755 $portable /opt/SereinGram/SereinGram
if ldd /opt/SereinGram/SereinGram | grep 'not found'; then exit $broken; fi"
fi

printf '%s\n%s' "$header" "$rows"
if [ "$failures" -gt 0 ]; then
	exit 1
fi
if [ "$passes" -eq 0 ]; then
	echo "::error title=Package test failed::No distribution could be checked on $arch."
	exit 1
fi
