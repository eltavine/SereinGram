"""Post the binaries of a SereinGram build to a Telegram channel.

The files go through the local Bot API server named by TELEGRAM_BOT_API
(telegram-bot-api --local), which lifts the 50 MB upload limit of the
public server. The channel first gets a message with the commit subject,
author, commit and workflow run links, then every binary of the release
policy as albums grouped by system, each replying to that message. When
no local server is given or posting the files fails, the public server
gets a plain text message with the same links and the download page
instead, which needs nothing but the bot token in TELEGRAM_BOT_TOKEN.
"""

import argparse
import html
import json
import os
import subprocess
import sys
import time
from pathlib import Path

import release

PUBLIC_API = "https://api.telegram.org"
ALBUM = 10
SUBJECT = 512
RETRIES = (5, 10, 20, 40)
STARTUP = (2,) * 30


class TelegramError(Exception):
    pass


def shorten(subject):
    return subject if len(subject) <= SUBJECT else subject[: SUBJECT - 1] + "\u2026"


def header(title, subject, author, commit, commit_url, run_number, run_url):
    lines = [
        f"<b>{html.escape(title)}</b>",
        "",
        f"<pre>{html.escape(shorten(subject))}</pre>",
        f"Author: <b>{html.escape(author)}</b>",
        f'Commit: <a href="{html.escape(commit_url)}">{html.escape(commit[:8])}</a>',
        f'Action: <a href="{html.escape(run_url)}">#{html.escape(run_number)}</a>',
    ]
    return "\n".join(lines)


def plain_text(title, subject, author, commit_url, run_url, release_url):
    lines = [
        title,
        "",
        shorten(subject),
        f"Author: {author}",
        f"Commit: {commit_url}",
        f"Action: {run_url}",
        f"Downloads: {release_url}",
    ]
    return "\n".join(lines)


def albums(directory, assets):
    systems = {}
    for asset in assets:
        if asset["kind"] in release.UPDATE_DATA:
            continue
        path = Path(directory) / asset["name"]
        if not path.is_file():
            raise TelegramError(f"{asset['name']}: missing from {directory}")
        systems.setdefault(asset["os"], []).append((asset["arch"], path))
    result = []
    for members in systems.values():
        parts = [members]
        if len(members) > ALBUM:
            arches = {}
            for arch, path in members:
                arches.setdefault(arch, []).append((arch, path))
            parts = list(arches.values())
        for part in parts:
            paths = [path for _, path in part]
            result += [paths[start : start + ALBUM] for start in range(0, len(paths), ALBUM)]
    return result


def call(api, token, method, fields, files=(), delays=RETRIES, sleep=time.sleep):
    command = ["curl", "--silent", "--show-error", "--max-time", "1800"]
    for key, value in fields.items():
        command += ["--form-string", f"{key}={value}"]
    for key, path in files:
        command += ["--form", f"{key}=@{path}"]
    command.append(f"{api}/bot{token}/{method}")
    problem = "no response"
    for attempt in range(len(delays) + 1):
        result = subprocess.run(command, capture_output=True, text=True, check=False)
        try:
            response = json.loads(result.stdout)
        except ValueError:
            response = None
        if not isinstance(response, dict):
            response = {}
        if response.get("ok"):
            return response["result"]
        code = response.get("error_code")
        problem = response.get("description") or result.stderr.strip() or "no response"
        if code is not None and 400 <= code < 500 and code != 429:
            break
        if attempt < len(delays):
            wait = delays[attempt]
            if code == 429:
                wait = response.get("parameters", {}).get("retry_after", wait)
            sleep(wait)
    raise TelegramError(f"{method} failed: {problem.replace(token, '***')}")


def post(api, token, chat, reply, paths):
    fields = {"chat_id": chat, "reply_parameters": reply}
    if len(paths) == 1:
        return call(api, token, "sendDocument", fields, [("document", paths[0])])
    media = [{"type": "document", "media": f"attach://file{index}"} for index in range(len(paths))]
    files = [(f"file{index}", path) for index, path in enumerate(paths)]
    return call(api, token, "sendMediaGroup", dict(fields, media=json.dumps(media)), files)


def commit_info(root, commit):
    output = subprocess.run(
        ["git", "log", "-1", "--format=%s%x00%an", commit],
        cwd=root,
        check=True,
        capture_output=True,
        text=True,
    ).stdout
    subject, author = output.rstrip("\n").split("\0", 1)
    return subject, author


def post_files(api, token, args, subject, author):
    groups = albums(args.directory, release.load_assets(args.assets))
    call(api, token, "getMe", {}, delays=STARTUP)
    text = header(
        args.title, subject, author, args.commit, args.commit_url, args.run_number, args.run_url
    )
    fields = {
        "chat_id": args.chat,
        "text": text,
        "parse_mode": "HTML",
        "link_preview_options": json.dumps({"is_disabled": True}),
    }
    message = call(api, token, "sendMessage", fields)
    message_id = message.get("message_id") if isinstance(message, dict) else None
    if not isinstance(message_id, int):
        raise TelegramError("sendMessage returned no message id")
    reply = json.dumps({"message_id": message_id, "allow_sending_without_reply": True})
    for paths in groups:
        post(api, token, args.chat, reply, paths)
        print(f"Posted {', '.join(path.name for path in paths)}.")


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("directory")
    parser.add_argument("--chat", required=True)
    parser.add_argument("--title", required=True)
    parser.add_argument("--commit", required=True)
    parser.add_argument("--commit-url", required=True)
    parser.add_argument("--run-number", required=True)
    parser.add_argument("--run-url", required=True)
    parser.add_argument("--release-url", required=True)
    parser.add_argument("--root", default=str(release.ROOT))
    parser.add_argument("--assets", default=str(release.POLICY))
    args = parser.parse_args(argv)
    token = os.environ.get("TELEGRAM_BOT_TOKEN", "")
    server = os.environ.get("TELEGRAM_BOT_API", "").rstrip("/")
    if not token:
        print("telegram_post.py: TELEGRAM_BOT_TOKEN is not set", file=sys.stderr)
        return 1
    try:
        subject, author = commit_info(args.root, args.commit)
    except (OSError, subprocess.CalledProcessError) as error:
        print(f"telegram_post.py: {error}", file=sys.stderr)
        return 1
    if server:
        try:
            post_files(server, token, args, subject, author)
            return 0
        except (TelegramError, release.ReleaseError, OSError) as error:
            print(
                f"::warning::Posting the binaries failed, so only a text message is sent: {error}"
            )
    else:
        print("::warning::No local Bot API server is running, so only a text message is sent.")
    text = plain_text(args.title, subject, author, args.commit_url, args.run_url, args.release_url)
    fields = {
        "chat_id": args.chat,
        "text": text,
        "link_preview_options": json.dumps({"is_disabled": True}),
    }
    try:
        call(PUBLIC_API, token, "sendMessage", fields)
    except TelegramError as error:
        print(f"telegram_post.py: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
