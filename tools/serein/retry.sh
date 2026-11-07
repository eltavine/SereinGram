#!/usr/bin/env bash
# Usage: retry.sh <attempts> <first wait in seconds> <command> [argument...]
set -euo pipefail

if [ $# -lt 3 ]; then
	echo "usage: $0 <attempts> <first wait in seconds> <command> [argument...]" >&2
	exit 2
fi
for number in "$1" "$2"; do
	case "$number" in
	'' | *[!0-9]*)
		echo "$0: the attempts and the wait must be whole numbers" >&2
		exit 2
		;;
	esac
done
attempts=$((10#$1))
wait=$((10#$2))
shift 2
if [ "$attempts" -lt 1 ]; then
	echo "$0: at least one attempt is needed" >&2
	exit 2
fi

attempt=1
while true; do
	status=0
	"$@" || status=$?
	if [ "$status" -eq 0 ]; then
		exit 0
	fi
	if [ "$attempt" -ge "$attempts" ]; then
		echo "$1 failed with exit code $status in all $attempts attempts." >&2
		exit "$status"
	fi
	echo "$1 failed with exit code $status (attempt $attempt of $attempts); retrying in $wait s." >&2
	sleep "$wait"
	attempt=$((attempt + 1))
	wait=$((wait * 2))
done
