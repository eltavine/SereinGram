import json
import os
import stat
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

SCRIPT = Path(__file__).resolve().parents[1] / "linux_package_test.sh"
FAKE_DOCKER = """\
#!/usr/bin/env python3
import json
import os
import sys

args = sys.argv[1:]
with open(os.environ["FAKE_DOCKER_LOG"], "a") as log:
    log.write(json.dumps(args) + "\\n")
plan = json.loads(os.environ.get("FAKE_DOCKER_PLAN", "{}"))
if args[0] == "rm":
    sys.exit(0)
if args[0] == "pull":
    outcome = plan.get("pull " + args[-1], 0)
else:
    image = args[args.index("bash") - 1]
    attempt = next(a.split("=", 1)[1] for a in args if a.startswith("PACKAGE_TEST_ATTEMPT="))
    outcome = plan.get(f"{image} {attempt}", plan.get(image, 0))
code, message = outcome if isinstance(outcome, list) else (outcome, "")
print(message, file=sys.stderr)
sys.exit(code)
"""
FAKE_TIMEOUT = """\
#!/bin/sh
while [ "${1#--}" != "$1" ]; do shift; done
shift
exec "$@"
"""
FAKE_ZYPPER = """\
#!/bin/sh
echo "$*" >> "$ZYPPER_LOG"
case "$*" in *" refresh") exit "${ZYPPER_REFRESH_STATUS:-0}" ;; esac
"""
REPOS = {
    "multi.repo": (
        "[extra-oss]\nenabled=0\nbaseurl=https://download.opensuse.org/tumbleweed/repo/oss/\n\n"
        "[extra-update]\nenabled=1\nbaseurl=http://download.opensuse.org/update/tumbleweed/"
    ),
    "openSUSE:repo-oss.repo": (
        "# Repository 'openSUSE:repo-oss' is maintained by the 'openSUSE' service.\n"
        "[openSUSE:repo-oss]\nname=repo-oss (${releasever})\nenabled=1\nautorefresh=1\n"
        "baseurl=http://cdn.opensuse.org/distribution/leap/${releasever}/repo/oss/$basearch\n"
        "service=openSUSE\n"
    ),
    "repo-debug.repo": (
        "[repo-debug]\nenabled=0\n"
        "baseurl=http://download.opensuse.org/ports/aarch64/debug/tumbleweed/repo/oss/\n"
    ),
    "repo-openh264.repo": (
        "[repo-openh264]\nenabled=1\nbaseurl=http://codecs.opensuse.org/openh264/openSUSE_Tumbleweed\n"
    ),
    "repo-oss.repo": (
        "[repo-oss]\nname=openSUSE-Tumbleweed-Oss\nenabled=1\nautorefresh=1\n"
        "baseurl=http://download.opensuse.org/ports/aarch64/tumbleweed/repo/oss/\n"
    ),
}
ARM64_IMAGES = [
    "debian:12-slim",
    "debian:13-slim",
    "ubuntu:22.04",
    "ubuntu:24.04",
    "ubuntu:26.04",
    "fedora:43",
    "opensuse/tumbleweed",
    "opensuse/leap:16.1",
]
X86_64_IMAGES = [*ARM64_IMAGES, "linuxmintd/mint22.3-amd64", "archlinux:latest"]


def executable(path, text):
    path.write_text(text)
    path.chmod(path.stat().st_mode | stat.S_IXUSR)


class Harness(unittest.TestCase):
    def setUp(self):
        self._temp = tempfile.TemporaryDirectory()
        self.dir = Path(self._temp.name)
        self.bin = self.dir / "bin"
        self.bin.mkdir()
        executable(self.bin / "docker", FAKE_DOCKER)
        executable(self.bin / "timeout", FAKE_TIMEOUT)
        executable(self.bin / "zypper", FAKE_ZYPPER)
        self.artifacts = self.dir / "artifact"
        self.artifacts.mkdir()
        self.log = self.dir / "docker.log"
        self.summary = self.dir / "summary.md"

    def tearDown(self):
        self._temp.cleanup()

    def environment(self, **extra):
        return dict(
            os.environ,
            PATH=f"{self.bin}{os.pathsep}{os.environ['PATH']}",
            FAKE_DOCKER_LOG=str(self.log),
            GITHUB_STEP_SUMMARY=str(self.summary),
            SEREIN_PACKAGE_TEST_WAIT="0",
            **extra,
        )

    def run_test(self, arch="arm64", plan=None):
        for extension in ("deb", "rpm", "tar.xz"):
            (self.artifacts / f"SereinGram-linux-{arch}.{extension}").write_bytes(b"x")
        return subprocess.run(
            ["bash", str(SCRIPT), str(self.artifacts), arch],
            capture_output=True,
            text=True,
            env=self.environment(FAKE_DOCKER_PLAN=json.dumps(plan or {})),
            check=False,
        )

    def calls(self, command):
        lines = self.log.read_text().splitlines() if self.log.exists() else []
        return [args for args in map(json.loads, lines) if args[0] == command]

    def runs_of(self, image):
        return [args for args in self.calls("run") if args[args.index("bash") - 1] == image]

    def script_of(self, image):
        args = self.runs_of(image)[0]
        return args[args.index("-c") + 1]


@unittest.skipIf(sys.platform == "win32", "runs a bash script")
class LinuxPackageTest(Harness):
    def test_checks_every_distribution_once(self):
        result = self.run_test("x86_64")
        self.assertEqual(result.returncode, 0, result.stderr)
        images = [args[args.index("bash") - 1] for args in self.calls("run")]
        self.assertEqual(images, X86_64_IMAGES)
        summary = self.summary.read_text()
        self.assertTrue(summary.startswith("### Linux package integration test (x86_64)\n"))
        self.assertEqual(summary.count("| passed |"), len(X86_64_IMAGES))

    def test_arm64_skips_the_x86_64_only_images(self):
        result = self.run_test("arm64")
        self.assertEqual(result.returncode, 0, result.stderr)
        images = [args[args.index("bash") - 1] for args in self.calls("run")]
        self.assertEqual(images, ARM64_IMAGES)

    def test_container_scripts_are_valid_bash(self):
        self.run_test("x86_64")
        for args in self.calls("run"):
            script = args[args.index("-c") + 1]
            syntax = subprocess.run(
                ["bash", "-n", "-c", script], capture_output=True, text=True, check=False
            )
            self.assertEqual(syntax.returncode, 0, syntax.stderr)
            self.assertTrue(script.startswith("unreachable=75\n"))
            self.assertIn("refresh ", script)
        self.assertIn("--error-on=any update", self.script_of("debian:12-slim"))

    def test_retries_a_failed_install_with_the_openSUSE_origin(self):
        result = self.run_test(plan={"opensuse/tumbleweed 1": 8})
        self.assertEqual(result.returncode, 0, result.stderr)
        runs = self.runs_of("opensuse/tumbleweed")
        self.assertEqual(len(runs), 2)
        self.assertIn("PACKAGE_TEST_ATTEMPT=2", runs[1])
        script = runs[1][runs[1].index("-c") + 1]
        self.assertLess(script.index("\norigin\n"), script.index("refresh zypper"))
        row = "| openSUSE Tumbleweed | `opensuse/tumbleweed` | passed in attempt 2 |"
        self.assertIn(row, self.summary.read_text())

    def test_unreachable_repositories_are_inconclusive(self):
        result = self.run_test(plan={"ubuntu:26.04": 75, "pull fedora:43": 1})
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual(len(self.runs_of("ubuntu:26.04")), 3)
        self.assertEqual(len(self.calls("pull")), len(ARM64_IMAGES) + 2)
        self.assertEqual(self.runs_of("fedora:43"), [])
        warning = "::warning title=Package test inconclusive::"
        self.assertIn(f"{warning}Ubuntu 26.04 (arm64)", result.stdout)
        self.assertIn(f"{warning}Fedora 43 (arm64)", result.stdout)
        self.assertEqual(self.summary.read_text().count("inconclusive"), 2)

    def test_a_timed_out_container_is_removed_and_retried(self):
        result = self.run_test(plan={"ubuntu:24.04": 124})
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual(len(self.runs_of("ubuntu:24.04")), 3)
        self.assertEqual(len(self.calls("rm")), 3)
        self.assertIn("Ubuntu 24.04 (arm64): the image or the package repositories", result.stdout)
        self.assertIn("unreachable or timed out", self.summary.read_text())

    def test_a_failing_distribution_fails_the_test(self):
        result = self.run_test(plan={"debian:13-slim": 1})
        self.assertEqual(result.returncode, 1)
        self.assertEqual(len(self.runs_of("debian:13-slim")), 3)
        self.assertIn("::error title=Package test failed::Debian 13 (arm64)", result.stdout)
        self.assertEqual(len(self.runs_of("opensuse/leap:16.1")), 1)

    def test_a_failure_before_an_outage_still_fails(self):
        plan = {"debian:12-slim 1": 100, "debian:12-slim 2": 75, "debian:12-slim 3": 75}
        result = self.run_test(plan=plan)
        self.assertEqual(result.returncode, 1)
        failure = "installing or checking the package failed with exit code 100 in attempt 1"
        self.assertIn(
            f"::error title=Package test failed::Debian 12 (arm64): {failure}.", result.stdout
        )
        self.assertIn(f"| failed, {failure} |", self.summary.read_text())

    def test_broken_packages_are_not_retried(self):
        result = self.run_test(plan={"fedora:43": 70})
        self.assertEqual(result.returncode, 1)
        self.assertEqual(len(self.runs_of("fedora:43")), 1)

    def test_a_missing_image_fails_without_retrying(self):
        result = self.run_test(plan={"pull ubuntu:26.04": [1, "manifest unknown"]})
        self.assertEqual(result.returncode, 1)
        pulls = [args for args in self.calls("pull") if args[-1] == "ubuntu:26.04"]
        self.assertEqual(len(pulls), 1)
        self.assertIn("manifest unknown", result.stderr)
        self.assertIn("Ubuntu 26.04 (arm64): the image does not exist.", result.stdout)

    def test_fails_when_no_distribution_could_be_checked(self):
        plan = {f"pull {image}": 1 for image in ARM64_IMAGES}
        result = self.run_test(plan=plan)
        self.assertEqual(result.returncode, 1)
        self.assertIn("No distribution could be checked on arm64.", result.stdout)

    def test_requires_every_package(self):
        (self.artifacts / "SereinGram-linux-arm64.deb").write_bytes(b"x")
        result = subprocess.run(
            ["bash", str(SCRIPT), str(self.artifacts), "arm64"],
            capture_output=True,
            text=True,
            check=False,
        )
        self.assertEqual(result.returncode, 2)
        self.assertIn("SereinGram-linux-arm64.rpm", result.stderr)


@unittest.skipIf(sys.platform == "win32", "runs a bash script")
class OpenSuseOriginTest(Harness):
    def refresh_repositories(self, attempt, refresh_status=0):
        self.run_test()
        script = self.script_of("opensuse/tumbleweed")
        prefix = script[: script.index("\n", script.index("refresh zypper")) + 1]
        repos = self.dir / "repos"
        repos.mkdir(exist_ok=True)
        for name, text in REPOS.items():
            (repos / name).write_text(text)
        log = self.dir / "zypper.log"
        log.unlink(missing_ok=True)
        result = subprocess.run(
            ["bash", "-euo", "pipefail", "-c", prefix],
            capture_output=True,
            text=True,
            env=self.environment(
                PACKAGE_TEST_ATTEMPT=str(attempt),
                SEREIN_ZYPP_REPOS=str(repos),
                ZYPPER_LOG=str(log),
                ZYPPER_REFRESH_STATUS=str(refresh_status),
            ),
            check=False,
        )
        return result, log.read_text().splitlines()

    def test_first_attempt_uses_the_configured_mirrors(self):
        result, calls = self.refresh_repositories(attempt=1)
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual(calls, ["--non-interactive --gpg-auto-import-keys refresh"])

    def test_later_attempts_replace_the_mirrors_with_the_origin(self):
        result, calls = self.refresh_repositories(attempt=2)
        self.assertEqual(result.returncode, 0, result.stderr)
        quiet = "--non-interactive --quiet"
        origin = "https://downloadcontent.opensuse.org"
        self.assertEqual(
            calls,
            [
                f"{quiet} modifyrepo --disable extra-update",
                f"{quiet} addrepo --priority 1 {origin}/update/tumbleweed/ origin-1",
                f"{quiet} modifyrepo --disable openSUSE:repo-oss",
                f"{quiet} addrepo --priority 1 "
                f"{origin}/distribution/leap/${{releasever}}/repo/oss/$basearch origin-2",
                f"{quiet} modifyrepo --disable repo-oss",
                f"{quiet} addrepo --priority 1 "
                f"{origin}/ports/aarch64/tumbleweed/repo/oss/ origin-3",
                "--non-interactive --gpg-auto-import-keys refresh",
            ],
        )

    def test_a_failed_refresh_reports_unreachable_repositories(self):
        result, _ = self.refresh_repositories(attempt=1, refresh_status=4)
        self.assertEqual(result.returncode, 75)


if __name__ == "__main__":
    unittest.main()
