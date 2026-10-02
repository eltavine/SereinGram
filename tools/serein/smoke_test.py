"""Start a built SereinGram binary and check that it keeps running.

The binary runs with a fresh -workdir, so no account or setting of the
machine is touched. The check passes when the log reports the launch and the
process is still alive after the settle time; the process is stopped either
way.
"""

import argparse
import subprocess
import sys
import tempfile
import time
from pathlib import Path

MARKER = "Launched version: "


def launched(workdir):
    for path in Path(workdir).rglob("*.txt"):
        try:
            if MARKER in path.read_text(encoding="utf-8", errors="replace"):
                return True
        except OSError:
            continue
    return False


def log_tail(workdir, lines=40):
    result = []
    for path in sorted(Path(workdir).rglob("*.txt")):
        text = path.read_text(encoding="utf-8", errors="replace")
        result.append(f"--- {path.name}")
        result += text.splitlines()[-lines:]
    return "\n".join(result)


def stop(process):
    if process.poll() is None:
        process.terminate()
        try:
            process.wait(timeout=15)
        except subprocess.TimeoutExpired:
            process.kill()
            process.wait()


def run(binary, timeout, settle, workdir):
    command = [str(binary), "-workdir", str(workdir), "-debug"]
    process = subprocess.Popen(command, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    try:
        deadline = time.monotonic() + timeout
        while not launched(workdir):
            if process.poll() is not None:
                return f"exited with code {process.returncode} before launch"
            if time.monotonic() > deadline:
                return f"no '{MARKER.strip()}' line within {timeout} s"
            time.sleep(1)
        time.sleep(settle)
        if process.poll() is not None:
            return f"exited with code {process.returncode} after launch"
        return None
    finally:
        stop(process)


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("binary")
    parser.add_argument("--timeout", type=int, default=90)
    parser.add_argument("--settle", type=int, default=20)
    args = parser.parse_args(argv)
    with tempfile.TemporaryDirectory(prefix="sereingram-smoke-") as workdir:
        error = run(args.binary, args.timeout, args.settle, workdir)
        if error:
            print(f"Smoke test failed: {error}", file=sys.stderr)
            print(log_tail(workdir), file=sys.stderr)
            return 1
    print("Smoke test passed: the app launched and kept running.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
