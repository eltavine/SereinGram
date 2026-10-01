#!/usr/bin/env bash
# Lays out the files that nfpm.yaml packages.
# Usage: stage.sh <SereinGram binary> <generated metainfo> <stage directory>
set -euo pipefail

if [ "$#" -ne 3 ]; then
	echo "Usage: $0 <binary> <metainfo> <stage directory>" >&2
	exit 2
fi
binary=$1
metainfo=$2
stage=$3
root=$(cd "$(dirname "$0")/../.." && pwd)
id=io.github.eltavine.SereinGram

rm -rf "$stage"
mkdir -p "$stage/icons/symbolic/apps"
cp "$binary" "$stage/SereinGram"
chmod 755 "$stage/SereinGram"
# The packages ship no D-Bus service file, so the entry must not ask for one.
sed '/^DBusActivatable=/d' "$root/lib/xdg/$id.desktop" > "$stage/$id.desktop"
cp "$metainfo" "$stage/$id.metainfo.xml"
for size in 16 32 48 64 128 256 512; do
	for scale in "" "@2"; do
		suffix=${scale:+@2x}
		mkdir -p "$stage/icons/${size}x${size}$scale/apps"
		cp "$root/Telegram/Resources/art/icon$size$suffix.png" \
			"$stage/icons/${size}x${size}$scale/apps/$id.png"
	done
done
cp "$root/Telegram/Resources/icons/tray_monochrome.svg" \
	"$stage/icons/symbolic/apps/$id-symbolic.svg"
