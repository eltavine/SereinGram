#!/usr/bin/env bash
# Installs the Linux packages in a container of every supported distribution
# and checks the installed files and the dynamic libraries they need.
#
# Usage: linux_package_test.sh <directory with the packages> <x86_64|arm64>
set -euo pipefail

if [ $# -ne 2 ]; then
	echo "usage: $0 <artifact directory> <x86_64|arm64>" >&2
	exit 2
fi
artifacts=$(cd "$1" && pwd)
arch=$2

deb="/artifact/SereinGram-linux-$arch.deb"
rpm="/artifact/SereinGram-linux-$arch.rpm"
tarball="/artifact/SereinGram-linux-$arch.tar.xz"

installed='test -x /usr/bin/SereinGram
test -f /usr/share/applications/io.github.eltavine.SereinGram.desktop
test -f /usr/share/metainfo/io.github.eltavine.SereinGram.metainfo.xml
test -f /usr/share/icons/hicolor/256x256/apps/io.github.eltavine.SereinGram.png
if ldd /usr/bin/SereinGram | grep "not found"; then exit 1; fi'

check() {
	local name=$1 image=$2 script=$3
	echo "::group::$name ($image)"
	docker run --rm -v "$artifacts:/artifact:ro" "$image" bash -euo pipefail -c "$script"
	echo "::endgroup::"
}

apt="export DEBIAN_FRONTEND=noninteractive
apt-get update -qq
apt-get install -y -qq --no-install-recommends $deb >/dev/null
$installed"
check "Debian 12" debian:12-slim "$apt"
check "Debian 13" debian:13-slim "$apt"
check "Ubuntu 22.04" ubuntu:22.04 "$apt"
check "Ubuntu 24.04" ubuntu:24.04 "$apt"
check "Ubuntu 26.04" ubuntu:26.04 "$apt"

check "Fedora 43" fedora:43 "dnf install -y -q $rpm
$installed"
zypper="zypper --non-interactive --quiet install --allow-unsigned-rpm $rpm
$installed"
check "openSUSE Tumbleweed" opensuse/tumbleweed "$zypper"
check "openSUSE Leap 16.1" opensuse/leap:16.1 "$zypper"

# The Linux Mint and Arch Linux images exist for x86_64 only. Arch users run
# the portable build or the AppImage; packaging/arch builds against system
# libraries in its own workflow.
if [ "$arch" = x86_64 ]; then
	check "Linux Mint 23" linuxmintd/mint23-amd64 "$apt"
	check "Arch Linux" archlinux:latest "pacman -Sy --noconfirm --needed \
	cairo fontconfig freetype2 glib2 pango >/dev/null
mkdir -p /opt
tar -xJf $tarball -C /opt
test -x /opt/SereinGram/SereinGram
if ldd /opt/SereinGram/SereinGram | grep 'not found'; then exit 1; fi"
fi
