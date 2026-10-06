# SereinGram

<img src="Telegram/Resources/branding/sereingram.png" width="128" alt="SereinGram app icon">

**SereinGram** is an unofficial, free and open-source Telegram client for
macOS, Windows and Linux. It is built on
[Telegram Desktop](https://github.com/telegramdesktop/tdesktop), keeps every
official Telegram feature and adds a large set of optional enhancements:
ghost mode, deleted messages and edit history, message filters, a
presentation mode, translation, transcription and AI services of your
choice, proxy tools and much more. Every enhancement starts switched off, so
until you turn something on SereinGram behaves like Telegram Desktop.

[![Nightly build](https://github.com/eltavine/SereinGram/actions/workflows/serein-nightly.yml/badge.svg?branch=develop)](https://github.com/eltavine/SereinGram/actions/workflows/serein-nightly.yml)
[![Download Nightly](https://img.shields.io/badge/download-nightly-2ea44f)](https://github.com/eltavine/SereinGram/releases/tag/nightly)
[![License: GPL v3](https://img.shields.io/badge/license-GPLv3-blue)](LICENSE)
[![Platforms: macOS, Windows, Linux](https://img.shields.io/badge/platforms-macOS%20%7C%20Windows%20%7C%20Linux-lightgrey)](#download)

> [!IMPORTANT]
> SereinGram is **not an official Telegram app**. It only draws on
> **Nagram** and **AyuGram** for reference and has **no affiliation of any
> kind** with their authors or maintainers, who neither endorse nor support
> it. Report SereinGram problems on the
> [SereinGram issue tracker](https://github.com/eltavine/SereinGram/issues),
> never to Telegram, Nagram or AyuGram, and read the
> [disclaimers](#disclaimers) before you use it.

中文用户请阅读[简体中文说明](#简体中文)。

## Contents

- [Highlights](#highlights)
- [Features](#features)
- [Download](#download)
- [Install](#install)
- [Updates](#updates)
- [First steps](#first-steps)
- [Build from source](#build-from-source)
- [Project layout](#project-layout)
- [Contributing](#contributing)
- [Privacy](#privacy)
- [Disclaimers](#disclaimers)
- [License](#license)
- [Credits](#credits)
- [简体中文](#简体中文)

## Highlights

- **All of Telegram Desktop.** Every official feature stays, and the code
  keeps up with upstream Telegram Desktop through a weekly sync.
- **Nothing changes until you want it to.** Every SereinGram option starts
  off, and the quick setup presets preview each change before applying it.
- **Privacy on your terms.** Ghost mode, a presentation mode for streaming
  and screen sharing, a settings lock and a masked phone number.
- **Keep what others delete.** Deleted messages and every edit are kept on
  your device, encrypted, and can be browsed like a chat.
- **Your own services.** Translation, transcription and AI providers of your
  choice, with their keys in the system keychain.
- **Runs everywhere.** Windows x86_64 and arm64, macOS on Apple silicon and
  Intel, and Linux x86_64 and arm64 as deb, rpm, AppImage, Flatpak, Snap,
  Arch Linux and Nix packages.
- **Transparent.** No telemetry or crash reporting, GPLv3 source code, and
  every published build comes from public CI.

## Features

Everything below is optional and lives in **Settings → SereinGram**. The
first time you open it, SereinGram offers **quick setup presets** (Clean
interface, Privacy first, Keep deleted and edited messages, Channel reading,
Technical details, Chinese typography) that list every change before they
apply it. The [feature matrix](docs/serein/features.md) (in Chinese) tracks
each feature with its source and status.

<details>
<summary>Ghost mode</summary>

- Do not send read receipts in private chats, groups, channels, topics,
  comments, mentions and reactions, while your own unread counters keep
  working.
- Do not mark stories as viewed and do not add views to channel posts.
- Stay offline: never send your online status, and go offline again right
  after sending a message.
- Hide typing, uploading and sticker-choosing activity.
- Turn it on per account or for all accounts, from the main menu, the tray
  menu or a keyboard shortcut; a 👻 in the chat header shows that it is on.
- Optionally send messages as scheduled ones so you never appear online,
  send silently, mark a chat as read after you reply, keep sending receipts
  in chats you pick, and send receipts up to a message on demand.

</details>

<details>
<summary>Deleted messages and edit history</summary>

- Keep messages that others delete in place, marked as deleted, also after
  a restart; downloaded and cached media stays viewable.
- Keep every edit of a message and browse its versions as a chat.
- Browse all deleted messages of a chat in their own view, with dates,
  senders and the original media where it is still available.
- Keep self-destructing photos and videos after their timer runs out
  (until you restart), and keep the history of groups and channels you were
  removed from.
- Everything is stored locally and encrypted, with retention limits,
  per-chat exclusions, optional bot messages and custom marks.

</details>

<details>
<summary>Filters, rules and alerts</summary>

- Regular expression filters that mask, replace or hide messages, for the
  whole account or for chosen chats and topics, with a rule tester, import,
  export and HTTPS rule subscriptions.
- Hide messages from blocked users, also in replies and in read and reaction
  lists; hide single messages on this device; filter Zalgo text.
- Link rules that fix URLs, strip tracking parameters, ask before opening
  links and can improve link previews.
- Keyword alerts that notify you even in muted chats.

</details>

<details>
<summary>Privacy and security</summary>

- A presentation mode that hides the window from screen capture and masks
  the chat list, chat titles and notifications; it can turn on by itself
  while OBS, Streamlabs, XSplit, vMix or similar apps run.
- Mask your own phone number, give users, groups and channels local names,
  and hide read-time and phone-sharing hints.
- Lock the SereinGram settings with your local passcode.
- Copy text and save media and screenshots of protected content on this
  device; Telegram still refuses to forward it.
- See the login time, API ID and official-app flag of every session, and
  scan QR codes from the screen, the clipboard or a file, including Telegram
  login codes behind a clear warning.
- Show contact relationships and an estimated registration date on
  profiles.

</details>

<details>
<summary>Appearance and notifications</summary>

- Bubble and avatar roundness, message width, wide channel posts, hidden
  bubble tails, theme colors for replies and rounded stickers.
- Main menu title, order and items, a custom app icon and halfwidth
  punctuation.
- Online status in the chat list, the chat header, profiles and member
  lists, with exact last-seen times if you want them.
- Notifications centered at the top of the screen, a notification delay,
  badge options and quiet hours with exceptions for contacts, pinned chats,
  mentions and keyword alerts.

</details>

<details>
<summary>Chats and folders</summary>

- A compact chat list, preview lines, hidden previews and stories, a
  startup folder, a hidden "All chats" folder, the archive inside folders
  and folder unread counters.
- Custom sorting, local pins, recent chats, remembered reading positions
  and a jump to the first message.
- Hide Premium promotions and birthday hints, and stay in the chat instead
  of jumping to the next channel or topic at its end.
- Chat menu shortcuts for search, shared media, pinned messages and
  clearing cached media, plus per-chat SereinGram settings.
- Search only the chats you already have, limit folders to chats you
  administer, and clean up deleted accounts, unused bots and silent groups
  and channels.

</details>

<details>
<summary>Messages</summary>

- Seconds in times, original forward dates, service message times, message
  IDs, exact counters, hidden view counts and channel signatures.
- Reaction visibility per chat type, no Premium sticker animations or
  message effects, revealed spoilers and a quick forward button.
- Message details with IDs, dates, the forward source and the sticker pack
  author, plus the data behind inline buttons.
- Spacing between Chinese and Latin text and Simplified and Traditional
  Chinese conversion with OpenCC while reading, and Persian calendar dates.
- Double-click your own messages to edit them.

</details>

<details>
<summary>Writing and sending</summary>

- Show or hide composer buttons, change the placeholder and insert bot
  commands instead of sending them.
- Turn off automatic Markdown, start without link previews, set a default
  code language, add spacing between Chinese and Latin text and save quick
  replies.
- Confirm before sending stickers, GIFs, voice and video messages or before
  calling.
- Forward first and comment afterwards, always send silently, and use text
  replacement rules and a formatting toolbar.
- Open links with inline bots you choose, post as the group in groups you
  own, and turn selected text into a mention.
- Translate drafts, or translate every message before sending in the chats
  you choose.

</details>

<details>
<summary>Message menu</summary>

- Show, hide or reveal with Option or Alt any built-in menu item.
- Repeat a message, repeat it without the quote or forward it without the
  quote, with an optional confirmation.
- Merge or reverse selected messages into a draft, save them to Saved
  Messages, select a range or one sender's messages, unpin a selection and
  select up to 1000 messages at once.
- Message screenshots, media info, copy as Markdown, reminders and quick
  reply texts.

</details>

<details>
<summary>Media, stickers and stories</summary>

- Sticker size, sticker times and up to 200 recent stickers; hide group and
  suggested stickers; greeting stickers.
- Video autoplay and GIF controls, MP4 files sent with a video preview,
  sticker pack export and import, and a sticker pack author lookup.
- Noise reduction for recorded voice messages and Force Touch previews on
  macOS.
- Post photo and video stories from the desktop, edit them and repost
  public stories, and download all media of a chat at once.

</details>

<details>
<summary>Translation, transcription and AI</summary>

- Bring your own services: OpenAI-compatible APIs, Anthropic, Gemini, DeepL,
  DeepLX, Google, Yandex, Microsoft Azure Translator and Transmart, with
  connection tests and keys in the system keychain.
- Translate drafts, messages, selected text and Instant View pages,
  translate whole chats without Premium through your own service, and
  translate chats automatically.
- Summaries of recent messages and context-aware translation with language
  models.
- Transcribe voice and video messages, automatically if you like, and draft
  with the system AI on macOS 26.

</details>

<details>
<summary>Network and proxies</summary>

- Proxy notes, sorting by latency, removal of broken proxies, proxy
  subscriptions and pausing the proxy while a VPN is on.
- A custom DNS-over-HTTPS server, the system DNS for proxy domains, faster
  uploads and downloads and a data center status check.
- Open Mini Apps as the Android client.

</details>

<details>
<summary>Accounts, groups and settings</summary>

- Up to 20 accounts on one device.
- Group management shortcuts in profile menus, deleting all your messages in
  a group, unblocking everyone, upgrading groups to supergroups and default
  choices in delete dialogs.
- Export and import settings with a preview of every change, back them up
  to Saved Messages and restore them on another device, reset everything to
  the defaults and search all SereinGram settings.
- An update check against GitHub Releases and the third-party licenses
  inside the app.

</details>

### What SereinGram does not do

- It never pretends to be an official Telegram app and never uses the API
  credentials of official apps.
- It does not fake Premium or any other server-side perk, does not hide
  logged-in accounts and does not export sessions, because an exported
  authorization key is as good as your password.
- It does not ship VMess, Shadowsocks, Trojan or other proxy cores; use an
  external proxy app with the built-in SOCKS5 support instead.
- It does not sync read state or message history between devices, which
  would need a server of its own.

## Download

No stable version has been released yet. The
[Nightly](https://github.com/eltavine/SereinGram/releases/tag/nightly)
pre-release is rebuilt from every push to `develop` once the complete CI
(guards, tests, every platform and every package) has passed, and it always
holds only the latest build. Stable releases will appear as `vX.Y.Z` on the
[releases page](https://github.com/eltavine/SereinGram/releases).

> [!WARNING]
> Until SereinGram's own API credentials are configured, Nightly builds are
> made with Telegram Desktop's public test credentials. Their release notes
> say so at the top, and logging in with them may be limited or fail.

File names never change, so the links below always point to the latest
Nightly. For the latest stable release, replace `download/nightly` with
`latest/download` in the address.

| System | Architecture | Package | File |
| --- | --- | --- | --- |
| Windows | x86_64 | Installer | [SereinGram-windows-x86_64-setup.exe](https://github.com/eltavine/SereinGram/releases/download/nightly/SereinGram-windows-x86_64-setup.exe) |
| Windows | x86_64 | Portable executable | [SereinGram-windows-x86_64-portable.exe](https://github.com/eltavine/SereinGram/releases/download/nightly/SereinGram-windows-x86_64-portable.exe) |
| Windows | arm64 | Installer | [SereinGram-windows-arm64-setup.exe](https://github.com/eltavine/SereinGram/releases/download/nightly/SereinGram-windows-arm64-setup.exe) |
| Windows | arm64 | Portable executable | [SereinGram-windows-arm64-portable.exe](https://github.com/eltavine/SereinGram/releases/download/nightly/SereinGram-windows-arm64-portable.exe) |
| macOS | universal | Disk image | [SereinGram-macos-universal.dmg](https://github.com/eltavine/SereinGram/releases/download/nightly/SereinGram-macos-universal.dmg) |
| macOS | arm64 | Disk image | [SereinGram-macos-arm64.dmg](https://github.com/eltavine/SereinGram/releases/download/nightly/SereinGram-macos-arm64.dmg) |
| macOS | x86_64 | Disk image | [SereinGram-macos-x86_64.dmg](https://github.com/eltavine/SereinGram/releases/download/nightly/SereinGram-macos-x86_64.dmg) |
| Linux | x86_64 | Portable executable | [SereinGram-linux-x86_64-portable](https://github.com/eltavine/SereinGram/releases/download/nightly/SereinGram-linux-x86_64-portable) |
| Linux | x86_64 | AppImage | [SereinGram-linux-x86_64.AppImage](https://github.com/eltavine/SereinGram/releases/download/nightly/SereinGram-linux-x86_64.AppImage) |
| Linux | x86_64 | Debian package | [SereinGram-linux-x86_64.deb](https://github.com/eltavine/SereinGram/releases/download/nightly/SereinGram-linux-x86_64.deb) |
| Linux | x86_64 | RPM package | [SereinGram-linux-x86_64.rpm](https://github.com/eltavine/SereinGram/releases/download/nightly/SereinGram-linux-x86_64.rpm) |
| Linux | x86_64 | Flatpak bundle | [SereinGram-linux-x86_64.flatpak](https://github.com/eltavine/SereinGram/releases/download/nightly/SereinGram-linux-x86_64.flatpak) |
| Linux | x86_64 | Snap package | [SereinGram-linux-x86_64.snap](https://github.com/eltavine/SereinGram/releases/download/nightly/SereinGram-linux-x86_64.snap) |
| Linux | x86_64 | Arch Linux package | [SereinGram-linux-x86_64.pkg.tar.zst](https://github.com/eltavine/SereinGram/releases/download/nightly/SereinGram-linux-x86_64.pkg.tar.zst) |
| Linux | arm64 | Portable executable | [SereinGram-linux-arm64-portable](https://github.com/eltavine/SereinGram/releases/download/nightly/SereinGram-linux-arm64-portable) |
| Linux | arm64 | AppImage | [SereinGram-linux-arm64.AppImage](https://github.com/eltavine/SereinGram/releases/download/nightly/SereinGram-linux-arm64.AppImage) |
| Linux | arm64 | Debian package | [SereinGram-linux-arm64.deb](https://github.com/eltavine/SereinGram/releases/download/nightly/SereinGram-linux-arm64.deb) |
| Linux | arm64 | RPM package | [SereinGram-linux-arm64.rpm](https://github.com/eltavine/SereinGram/releases/download/nightly/SereinGram-linux-arm64.rpm) |
| Linux | arm64 | Flatpak bundle | [SereinGram-linux-arm64.flatpak](https://github.com/eltavine/SereinGram/releases/download/nightly/SereinGram-linux-arm64.flatpak) |

Every release also carries `SHA256SUMS` and a machine-readable
`release.json`. The naming scheme, the manifest format and the CI behind
each build are documented in the [release conventions](docs/serein/releases.md)
(in Chinese).

## Install

Builds are not signed with a developer certificate or notarized yet, so
each system asks you to confirm the first start. You can check every file
against `SHA256SUMS` first, as described under
[Verify your download](#verify-your-download).

### Windows

1. Download the installer (`-setup.exe`) or the portable executable
   (`-portable.exe`) for x86_64 or arm64.
2. If SmartScreen warns about an unrecognized app, choose **More info**,
   then **Run anyway**.
3. The installer adds SereinGram to the Start menu. The portable executable
   runs from anywhere; create a folder named `SereinGramForcePortable` next
   to it to keep all data beside the executable.

### macOS

1. Choose `SereinGram-macos-universal.dmg`, which runs on Apple silicon and
   Intel Macs, or the smaller `-arm64` (Apple silicon) or `-x86_64` (Intel)
   image.
2. Open the disk image and drag SereinGram to Applications.
3. The app only carries an ad-hoc signature, so macOS blocks the first
   launch. Try to open it once, then go to **System Settings → Privacy &
   Security** and choose **Open Anyway**.

### Linux

SereinGram supports Ubuntu, Debian, Linux Mint, Fedora, openSUSE, Arch Linux
and NixOS directly and runs on any distribution with glibc 2.28 or newer
through the AppImage, the Flatpak bundle or the portable executable. On
64-bit ARM, use `arm64` instead of `x86_64` in the file names.

| Distribution | Package | Install with |
| --- | --- | --- |
| Ubuntu, Debian, Linux Mint and derivatives | `.deb` | `sudo apt install ./SereinGram-linux-x86_64.deb` |
| Fedora, RHEL, Rocky Linux and AlmaLinux 8 or newer | `.rpm` | `sudo dnf install ./SereinGram-linux-x86_64.rpm` |
| openSUSE Tumbleweed and Leap | `.rpm` | `sudo zypper install --allow-unsigned-rpm ./SereinGram-linux-x86_64.rpm` |
| Arch Linux, Manjaro, EndeavourOS, CachyOS | `.pkg.tar.zst` (x86_64) | `sudo pacman -U ./SereinGram-linux-x86_64.pkg.tar.zst` |
| Distributions with Snap | `.snap` (x86_64) | `sudo snap install --dangerous ./SereinGram-linux-x86_64.snap` |
| Any distribution | AppImage | `chmod +x SereinGram-linux-x86_64.AppImage`, then run it |
| Any distribution | Portable executable | `chmod +x SereinGram-linux-x86_64-portable`, then run it |
| Any distribution with Flatpak | `.flatpak` | see [Flatpak](#flatpak) |
| NixOS and any system with Nix | Nix flake | see [NixOS and Nix](#nixos-and-nix) |

The `.deb` and `.rpm` packages are tested in CI on Debian 12 and 13, Ubuntu
22.04, 24.04 and 26.04, Linux Mint 22.3, Fedora 43 and openSUSE Tumbleweed
and Leap 16.1. The [Linux guide](docs/serein/linux.md) (in Chinese) has all
details.

#### Flatpak

The bundle uses the GNOME 51 runtime from Flathub:

```sh
flatpak remote-add --user --if-not-exists flathub https://dl.flathub.org/repo/flathub.flatpakrepo
flatpak install --user --bundle ./SereinGram-linux-x86_64.flatpak
flatpak run io.github.eltavine.SereinGram
```

#### NixOS and Nix

`flake.nix` exports `packages.<system>.sereingram` (also `default`) and
`overlays.default` for `x86_64-linux` and `aarch64-linux`. SereinGram never
ships API credentials, so the package needs your own `api_id` and
`api_hash` from <https://my.telegram.org>:

```nix
sereingram.packages.${pkgs.stdenv.hostPlatform.system}.default.override {
  apiId = 12345;
  apiHash = "your api_hash";
}
```

The flake needs Nix 2.27 or newer. Outside NixOS, start the app through
[nixGL](https://github.com/nix-community/nixGL) or with
`QT_XCB_GL_INTEGRATION=none`, because Nix's OpenGL libraries cannot find the
system graphics driver.

### Verify your download

`SHA256SUMS` uses the GNU coreutils format. Put it next to the downloaded
files and run:

```sh
# Linux
sha256sum --ignore-missing -c SHA256SUMS
# macOS
shasum -a 256 --ignore-missing -c SHA256SUMS
```

On Windows, compare the output of
`(Get-FileHash .\SereinGram-windows-x86_64-setup.exe).Hash` in PowerShell
with the matching line; letter case does not matter.

## Updates

- **In-app check.** With **Check for updates on GitHub** on (in SereinGram's
  interface settings, on by default), SereinGram reads the release manifest
  30 seconds after start and then once a day. Nightly builds follow the
  Nightly, all other builds follow stable releases. When an update is out
  you get the release page and a direct download for your kind of
  installation; SereinGram never downloads or replaces itself. The header
  of **Settings → SereinGram** also lets you check right away.
- **AppImage.** The AppImage carries zsync update information, so
  AppImageUpdate, AppImageLauncher or Gear Lever can update it by
  downloading only what changed.
- **Packages built from source.** Builds that use system libraries, such as
  the PKGBUILD and the Nix package, are updated by your package manager and
  do not check GitHub. The Flatpak, Snap and Arch Linux packages published
  on GitHub do check it, since no repository updates them.
- Telegram Desktop's own updater and crash reporter are turned off at build
  time; SereinGram never contacts Telegram's update servers.

## First steps

- SereinGram keeps its data in its own `SereinGram` folder (on macOS in
  `~/Library/Application Support/SereinGram`) and never reads Telegram
  Desktop or Nagram profiles, so you log in once more. It can run beside
  Telegram Desktop; both handle `tg://` links, and your system decides
  which one opens them.
- Open **Settings → SereinGram**. On the first visit a welcome dialog
  explains that everything is off and offers the quick setup presets. The
  header shows your version and whether an update is available.
- The **About** page is in the main menu, next to the version, and under
  **Settings → SereinGram → About SereinGram**. It holds the disclaimers,
  the privacy notes, the license, the credits and the third-party licenses.
- Shortcuts for ghost mode, the presentation mode and recent chats are
  listed at the end of the keyboard shortcuts settings; they have no keys
  until you record some.

## Build from source

### Before you start

- **API credentials.** Get your own `api_id` and `api_hash` at
  <https://my.telegram.org/apps>. SereinGram never uses the credentials of
  official Telegram apps and the repository contains none, so a build
  without credentials stops with an error. Provide them in one of these
  ways:
  - the environment variables `SEREIN_API_ID` and `SEREIN_API_HASH` while
    configuring;
  - a copy of `Telegram/build/api_credentials.local.cmake.example` named
    `Telegram/build/api_credentials.local.cmake`, which Git ignores;
  - `-D TDESKTOP_API_ID=...` and `-D TDESKTOP_API_HASH=...`, used only while
    neither of the above is set.

  For quick local experiments, `-D TDESKTOP_API_TEST=ON` selects Telegram
  Desktop's public test credentials, which the server limits heavily and
  which are meant for development only.
- **Toolchain.** SereinGram builds exactly like Telegram Desktop. Follow the
  upstream guides for [macOS](docs/building-mac.md),
  [Windows](docs/building-win.md) and [Linux](docs/building-linux.md), but
  clone `https://github.com/eltavine/SereinGram.git` instead of
  `tdesktop`. A full macOS build needs about 55 GB of free disk space.
- **Output.** The CMake target is still called `Telegram`; the program is
  `SereinGram.app`, `SereinGram.exe` or `SereinGram` in `out/<configuration>`.

### Build on macOS

With Xcode and the Homebrew packages from the macOS guide installed:

```sh
mkdir -p ~/TBuild && cd ~/TBuild
git clone --recursive https://github.com/eltavine/SereinGram.git
./SereinGram/Telegram/build/prepare/mac.sh
cd SereinGram/Telegram
SEREIN_API_ID=YOUR_API_ID SEREIN_API_HASH=YOUR_API_HASH ./configure.sh
cmake --build ../out --config Debug --target Telegram
```

### Build on Windows

In an **x64 Native Tools Command Prompt for VS 2026** started with
`-vcvars_ver=14.44`, inside a build folder that has `ThirdParty` and
`Libraries` subfolders:

```bat
git clone --recursive https://github.com/eltavine/SereinGram.git
SereinGram\Telegram\build\prepare\win.bat
cd SereinGram\Telegram
configure.bat x64 -D TDESKTOP_API_ID=YOUR_API_ID -D TDESKTOP_API_HASH=YOUR_API_HASH
cmake --build ..\out --config Debug --target Telegram
```

### Build on Linux

The Linux build runs in the same Rocky Linux 8 Docker image as Telegram
Desktop, which needs Docker and Poetry:

```sh
git clone --recursive https://github.com/eltavine/SereinGram.git
cd SereinGram
Telegram/build/prepare/linux.sh
export SEREIN_API_ID=YOUR_API_ID SEREIN_API_HASH=YOUR_API_HASH
docker run --rm -it \
    -u "$(id -u)" \
    -v "$PWD:/usr/src/tdesktop" \
    -e CONFIG=Debug \
    -e SEREIN_API_ID \
    -e SEREIN_API_HASH \
    tdesktop:centos_env \
    /usr/src/tdesktop/Telegram/build/docker/centos_env/build.sh
```

Drop `-e CONFIG=Debug` for a Release build.

### Distribution packages

| Package | Recipe | Credentials |
| --- | --- | --- |
| Arch Linux | `packaging/arch/PKGBUILD` (`sereingram-desktop-git`, system libraries) | `SEREIN_API_ID` and `SEREIN_API_HASH` for `makepkg -si` |
| Flatpak | `packaging/flatpak/io.github.eltavine.SereinGram.yml`, built with `flatpak-builder` | `Telegram/build/api_credentials.local.cmake` |
| Nix | `flake.nix` and `packaging/nix/package.nix` | the `apiId` and `apiHash` arguments |
| Snap | `snap/snapcraft.yaml` (core24) | `-DTDESKTOP_API_ID` and `-DTDESKTOP_API_HASH` in a local copy of `cmake-parameters`, never committed |
| deb and rpm | `packaging/nfpm/`, packing the static build | the credentials of that build |

`tools/serein/check_packaging.py` rejects committed credentials and keeps
the pinned dependency versions of all recipes in step.

### Checks and tests

`tools/serein/check_all.sh` runs every check that does not need the full
application build: the core tests (also under AddressSanitizer and
UndefinedBehaviorSanitizer), clang-tidy, formatting and style rules, ruff,
shellcheck, yamllint, markdownlint, `buf lint` and `buf breaking`, generated
code drift, module boundaries, the upstream intrusion budget, the feature
matrix and the workflow linters. It needs CMake, Ninja, Qt 6 (Homebrew's
`qtbase` on macOS), OpenSSL 3, uv, Buf and Node.js. The core tests alone:

```sh
cmake -S tools/serein/core_tests -B out/serein-core-tests -G Ninja \
    -DCMAKE_PREFIX_PATH="$(brew --prefix qtbase)"
cmake --build out/serein-core-tests && ctest --test-dir out/serein-core-tests
```

A full application build also produces `out/serein-tests/<configuration>/test_serein`.

## Project layout

| Path | Contents |
| --- | --- |
| `Telegram/SourceFiles/serein/` | SereinGram's own code: core, ports, adapters, hooks, features, settings and the composition root |
| `Telegram/Resources/langs/serein/` | English source strings and the translations |
| `proto/` | proto3 schema for settings, structured configuration, exports and history |
| `tools/serein/` | guards, code generators, release, packaging and CI helpers |
| `packaging/`, `snap/`, `flake.nix` | Arch Linux, Flatpak, Nix, nFPM and Snap recipes |
| `docs/serein/` | product specification, feature matrix, architecture, release conventions and ADRs (in Chinese) |
| `.github/workflows/serein-*.yml` | CI, Nightly, release, packaging, translation and upstream sync workflows |

The [architecture](docs/serein/architecture.md) keeps SereinGram easy to
merge with Telegram Desktop:

- Upstream files only include headers from `serein/hooks/`, and each hook
  is a single call or condition; the logic lives under `serein/`.
- Options are declared in proto3, and their code, including most of the
  settings pages, is generated; defaults always match Telegram's behavior.
- CI enforces module boundaries, a 1000-line limit per SereinGram file and
  a budget for changes to upstream files.
- `serein-upstream.yml` merges Telegram Desktop's `dev` branch every week
  and opens a pull request that must pass all three platform builds.

## Contributing

Pull requests go to `develop`, the default branch; `main` only receives
release merges. Before adding a feature, read the
[product specification](docs/serein/README.md) and the architecture, then:

- register the feature in the [feature matrix](docs/serein/features.md);
- declare its options in `proto/` and regenerate the code, with defaults
  that keep Telegram's behavior;
- reach upstream code only through one-line hooks in `serein/hooks/`;
- add core tests and strings in English, Simplified and Traditional Chinese.

### Commit messages

Commits follow [Conventional Commits](https://www.conventionalcommits.org/)
as `type(scope): summary`, written in English with printable ASCII only.
The subject has at most 72 characters, and the body explains why the change
was made and what it changes in at least 60 characters, with lines of at
most 100 characters. CI checks every new commit with commitlint and
`tools/serein/commitlint.config.mjs`. Check messages locally with:

```sh
git config core.hooksPath tools/serein/githooks
python3 tools/serein/check_commit_message.py path/to/message.txt
```

### Translations

SereinGram's own strings live in `Telegram/Resources/langs/serein/`:
`serein.strings` is the English source, and `zh-hans.strings` and
`zh-hant.strings` must stay complete. Other languages may be partial and
fall back to English; once the Crowdin project is configured,
`serein-crowdin.yml` uploads the source and opens weekly pull requests with
the translations. Telegram's own strings come from the official
[translation platform](https://translations.telegram.org).

### Reporting problems

Open an issue on the
[SereinGram issue tracker](https://github.com/eltavine/SereinGram/issues)
with the version from the About page, your system, the steps that lead to
the problem and, if possible, whether it also happens with every
SereinGram option off. Problems that also happen in the official Telegram
Desktop belong to Telegram Desktop. Never send SereinGram problems to
Telegram, Nagram or AyuGram.

## Privacy

- SereinGram adds no analytics, telemetry, ads or crash reporting.
  Telegram Desktop's crash reporter and updater are disabled at build time.
- Besides the connections Telegram Desktop itself makes, SereinGram only
  contacts GitHub for the update check, which you can turn off, and the
  services you set up yourself: translation, transcription and AI
  providers, rule and proxy subscriptions, a custom DNS-over-HTTPS server
  or a sticker author lookup bot.
- External services receive only the text or audio you ask them to process,
  including from automatic features you turn on, and handle it under their
  own privacy policies. Their keys stay in the macOS Keychain, the Windows
  Credential Manager or, on other systems, a file encrypted with the local
  key, and are never part of a settings export.
- Deleted messages and edit history are kept only if you turn that on.
  They stay on your device, encrypted with AES-256-GCM under a key derived
  from the account's local key, and are erased when you log out.

## Disclaimers

- **Unofficial client.** SereinGram is an independent third-party app. It
  is not developed, endorsed, sponsored or supported by Telegram, and it is
  not affiliated with Telegram or the Telegram Desktop developers. It talks
  to Telegram through the public [Telegram API](https://core.telegram.org/api),
  never uses the API credentials of official Telegram apps and never
  presents itself as one.
- **No affiliation with Nagram or AyuGram.** SereinGram only draws on
  [Nagram](https://github.com/NextAlone/Nagram) and
  [AyuGram](https://github.com/AyuGram/AyuGramDesktop) for reference. It
  has no affiliation of any kind with the authors or maintainers of either
  project: it is not their work, it is not developed together with them,
  and they neither endorse, review nor support it. Do not contact them
  about SereinGram. The same applies to the other clients SereinGram has
  looked at, such as Swiftgram, NekoX and Nekogram.
- **Origin of the code.** SereinGram's code base started as a fork of
  [Nagram-qt](https://github.com/NextAlone/Nagram-qt), NextAlone's
  open-source fork of Telegram Desktop. It was then restructured around
  Telegram Desktop as its only upstream
  ([ADR-0001](docs/serein/adr/0001-upstream-and-intrusion.md)); the parts
  of that code still in use remain under the GPLv3, with their authorship
  kept in the Git history. Building on GPLv3 code does not create any
  affiliation.
- **Terms of Service and your account.** Using Telegram through SereinGram
  is still subject to the [Telegram Terms of Service](https://telegram.org/tos)
  and the [Telegram API Terms of Service](https://core.telegram.org/api/terms).
  Optional features such as ghost mode, keeping deleted messages and expired
  self-destructing media, or saving protected content may go against those
  terms. They are all off by default; turning them on is your own decision
  and at your own risk, including any limits Telegram places on your
  account.
- **Respect others.** Keeping messages that others deleted, reading without
  sending read receipts or capturing protected content can go against what
  the people you talk to expect. Respect their privacy and follow the laws
  that apply to you.
- **No warranty.** SereinGram is distributed in the hope that it will be
  useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, as the GNU GPL v3
  states. Its developers are not liable for lost data, restricted accounts
  or any other damage caused by using it. Nightly builds are development
  versions and may be unstable.
- **Names and trademarks.** Telegram, Nagram, AyuGram and the other names
  mentioned here belong to their respective owners and are used only to
  describe where SereinGram comes from and what it works with. SereinGram
  has its own name and logo and uses no Telegram or Nagram artwork for its
  identity.
- **Third-party services.** Translation, transcription, AI and other
  services you add are run by their providers under their own terms;
  SereinGram does not control them and is not responsible for them.

## License

SereinGram is free software: you can redistribute it and modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation, version 3 or any later version, with the OpenSSL
exception of Telegram Desktop. See [LICENSE](LICENSE) and [LEGAL](LEGAL).

- Telegram Desktop: copyright © 2014–2026 The Telegram Desktop Authors.
- SereinGram: copyright © 2026 The SereinGram Authors, as recorded in the
  Git history.
- Bundled components keep their own licenses: OpenCC (Apache-2.0),
  marisa-trie (BSD-2-Clause or LGPL-2.1-or-later), darts-clone
  (BSD-2-Clause), RapidJSON (MIT), quirc (ISC) and the WizardLoop
  CreationDate data (MIT). Their full texts are in the app under
  **Settings → SereinGram → Third-party licenses** and on the About page.
  The libraries Telegram Desktop itself uses keep their licenses as well.

## Credits

- [Telegram Desktop](https://github.com/telegramdesktop/tdesktop) and its
  authors, whose work SereinGram is built on.
- NextAlone, whose [Nagram-qt](https://github.com/NextAlone/Nagram-qt) was
  the starting point of the code base.
- [Nagram](https://github.com/NextAlone/Nagram) and
  [AyuGram](https://github.com/AyuGram/AyuGramDesktop), used as references
  for many features, without any affiliation.
- [OukaroMF](https://github.com/OukaroMF/), who designed the SereinGram
  logo ([brand assets](BRANDING.md)).
- OpenCC, marisa-trie, darts-clone, RapidJSON, quirc, WizardLoop
  CreationDate, doctest and every other open-source project SereinGram
  depends on.

## 简体中文

### 简介

SereinGram 是一款基于 [Telegram Desktop](https://github.com/telegramdesktop/tdesktop) 的非官方、自由开源 Telegram 桌面客户端，支持 macOS、Windows 与主流 Linux 发行版。它保留 Telegram 的全部官方功能，并加入幽灵模式、已删除消息与编辑历史、消息过滤、主播模式、自选的翻译、转写与 AI 服务、代理工具等大量可选增强。所有增强默认关闭；什么都不开启时，SereinGram 的行为与 Telegram Desktop 一致。

产品规格、架构与发布约定的中文文档见 [docs/serein](docs/serein/README.md)，逐项功能与状态见[功能矩阵](docs/serein/features.md)。

### 下载与安装

- 目前尚未发布正式版。[Nightly](https://github.com/eltavine/SereinGram/releases/tag/nightly) 在每次推送到 `develop` 并通过全部 CI 后更新，始终只包含最新一次构建。
- 在 SereinGram 配置自己的 API 凭据之前，Nightly 使用 Telegram Desktop 公开的测试凭据构建，发布说明顶部会注明，登录可能受限或失败。
- Windows：安装包或便携版。程序尚未签名，SmartScreen 提示时选择“更多信息”，再选“仍要运行”。
- macOS：通用版，或 Apple 芯片、Intel 专用磁盘映像。程序只有 ad-hoc 签名、未经公证，首次打开被拦截后，到“系统设置 → 隐私与安全性”中选择“仍要打开”。
- Linux：`.deb`、`.rpm`、AppImage、便携版、Flatpak、Snap、Arch Linux 软件包与 Nix flake，详见 [Linux 发行版](docs/serein/linux.md)。
- 下载后可用 `SHA256SUMS` 校验，方法见上文的 [Verify your download](#verify-your-download)。

### 声明

- **非官方客户端**：SereinGram 是独立的第三方应用，并非由 Telegram 开发，也未获得 Telegram 的认可、赞助或支持，与 Telegram 及 Telegram Desktop 的开发者没有关联。它通过公开的 Telegram API 连接，从不使用 Telegram 官方应用的 API 凭据，也从不冒充官方应用。
- **与 Nagram、AyuGram 无任何关联**：SereinGram 只是借鉴了 Nagram 与 AyuGram，与这两个项目的作者及维护者没有任何关联：它不是他们的作品，没有与他们合作开发，也未获得他们的认可、审核或支持。请不要就 SereinGram 的任何问题联系他们。SereinGram 参考过的其他客户端（如 Swiftgram、NekoX、Nekogram）同样与本项目无关。
- **代码来源**：SereinGram 的代码库最初分支自 NextAlone 基于 Telegram Desktop 的开源分支 [Nagram-qt](https://github.com/NextAlone/Nagram-qt)，此后改为以 Telegram Desktop 为唯一上游并重新组织；其中仍在使用的代码继续以 GPLv3 授权，作者信息保留在 Git 历史中。基于 GPLv3 代码开发并不构成任何关联。
- **服务条款与账号风险**：通过 SereinGram 使用 Telegram 时，仍须遵守 [Telegram 服务条款](https://telegram.org/tos)与 [Telegram API 服务条款](https://core.telegram.org/api/terms)。幽灵模式、保留已删除的消息与已过期的限时媒体、保存受保护内容等可选功能可能违反这些条款。它们默认全部关闭；是否开启由你自行决定并自担风险，包括 Telegram 因此对账号施加的任何限制。
- **尊重他人**：保留他人已删除的消息、在不发送已读回执的情况下阅读或截取受保护内容，都可能违背对方的预期。请尊重他人的隐私，并遵守适用于你的法律。
- **无担保**：SereinGram 按“现状”提供，在法律允许的范围内不附带任何明示或默示的担保。开发者不对因使用本软件造成的数据丢失、账号受限或其他任何损失承担责任。Nightly 构建是开发版本，可能不稳定。
- **名称与商标**：Telegram、Nagram、AyuGram 以及本文提到的其他名称归各自所有者所有，仅用于说明 SereinGram 的来源与兼容对象。SereinGram 使用自己的名称与图标，没有使用 Telegram 或 Nagram 的图形作为应用标识。
- **隐私**：SereinGram 没有加入任何统计、遥测、广告或崩溃报告。除 Telegram Desktop 本身的连接外，它只会连接 GitHub 检查更新（可在 SereinGram 的界面设置中关闭），以及你自己设置的服务。

### 许可与致谢

SereinGram 以 GNU GPL 第 3 版（或任何更新的版本）授权，并沿用 Telegram Desktop 的 OpenSSL 例外条款，见 [LICENSE](LICENSE) 与 [LEGAL](LEGAL)。感谢 Telegram Desktop 的作者们、Nagram-qt 的作者 NextAlone、图标设计者 [OukaroMF](https://github.com/OukaroMF/)，以及 OpenCC、marisa-trie、darts-clone、RapidJSON、quirc、WizardLoop CreationDate 等开源项目。问题与建议请提交到 [GitHub Issues](https://github.com/eltavine/SereinGram/issues)。
