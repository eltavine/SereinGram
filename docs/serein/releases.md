# SereinGram 发布与构建产物约定

本文是脚本、包管理器与自动更新可以长期依赖的发布约定。约定只做向后兼容的扩展：可以增加新的产物，已发布的产物名称、校验文件格式与清单字段不改名、不删除。

## 1. 渠道

| 渠道 | 标签 | 生成方式 | Release 状态 |
| --- | --- | --- | --- |
| Nightly | `nightly`（滚动） | 每天 19:00 UTC 由 `serein-release.yml` 从 `develop` 最新提交构建；分支自上次 Nightly 以来没有新提交时跳过；也可手动触发，`develop` 上修改该工作流本身时同样运行 | 预发布，每次构建替换上一次 |
| 正式版 | `vX.Y.Z`，预发布为 `vX.Y.Z-<后缀>` | 推送标签时由 `serein-release.yml` 从标签提交构建 | 草稿，核对后手动发布；带后缀的标签标为预发布 |

固定下载地址：

- Nightly：`https://github.com/eltavine/SereinGram/releases/download/nightly/<产物名>`
- 最新正式版：`https://github.com/eltavine/SereinGram/releases/latest/download/<产物名>`
- 指定版本：`https://github.com/eltavine/SereinGram/releases/download/vX.Y.Z/<产物名>`

默认分支是 `develop`，定时触发直接运行其中的 `serein-release.yml`；也可以在 Actions 页面手动运行。`main` 只接收发布合并，受分支规则保护。

## 2. 产物

所有产物都是 Release 配置构建，当前阶段不使用开发者证书签名、不做公证。macOS 应用包只带不需要证书的 ad-hoc 签名，这是 Apple 芯片启动程序的前提。因此第一次启动时，macOS 需要先打开一次应用，再到“系统设置 > 隐私与安全性”中选择“仍要打开”；Windows 需要在 SmartScreen 提示中选择“更多信息”和“仍要运行”。发布页正文的“Verify”一节给出同样的说明。

命名规则：`SereinGram-<系统>-<架构>[-<变体>].<扩展名>`，系统为 `windows`、`macos`、`linux`，架构为 `x86_64`、`arm64`、`universal`。各渠道的同一产物名称相同，渠道与版本由标签区分。完整列表以 `tools/serein/policy/release_assets.json` 为准，发布前逐一核对，缺少或多出任何文件都会让发布失败。守卫把这份清单与 PR 目标分支或推送前的版本比较，删除、改名或改变已有产物的系统、架构与类型都会让检查失败，只允许新增。

| 系统 | 架构 | 产物 | 内容 |
| --- | --- | --- | --- |
| Windows | x86_64 | `SereinGram-windows-x86_64-setup.exe` | Inno Setup 安装包 |
| Windows | x86_64 | `SereinGram-windows-x86_64-portable.zip` | 便携版，`SereinGram/SereinGram.exe` |
| Windows | arm64 | `SereinGram-windows-arm64-setup.exe` | Inno Setup 安装包 |
| Windows | arm64 | `SereinGram-windows-arm64-portable.zip` | 便携版，`SereinGram/SereinGram.exe` |
| macOS | universal | `SereinGram-macos-universal.dmg` | arm64 与 x86_64 通用应用 |
| macOS | arm64 | `SereinGram-macos-arm64.dmg` | Apple 芯片专用应用，只含 arm64 代码 |
| macOS | x86_64 | `SereinGram-macos-x86_64.dmg` | Intel 处理器专用应用，只含 x86_64 代码 |
| Linux | x86_64 | `SereinGram-linux-x86_64.tar.xz` | 便携版，`SereinGram/SereinGram`（glibc 2.28 起） |
| Linux | x86_64 | `SereinGram-linux-x86_64.AppImage` | AppImage |
| Linux | x86_64 | `SereinGram-linux-x86_64.deb` | Debian、Ubuntu 软件包 |
| Linux | x86_64 | `SereinGram-linux-x86_64.rpm` | Fedora、openSUSE 等软件包 |
| Linux | x86_64 | `SereinGram-linux-x86_64.flatpak` | Flatpak 单文件包（GNOME 51 运行时） |
| Linux | arm64 | `SereinGram-linux-arm64.tar.xz` | 便携版，`SereinGram/SereinGram`（glibc 2.28 起） |
| Linux | arm64 | `SereinGram-linux-arm64.AppImage` | AppImage |
| Linux | arm64 | `SereinGram-linux-arm64.deb` | Debian、Ubuntu 软件包 |
| Linux | arm64 | `SereinGram-linux-arm64.rpm` | Fedora、openSUSE 等软件包 |
| Linux | arm64 | `SereinGram-linux-arm64.flatpak` | Flatpak 单文件包（GNOME 51 运行时） |

macOS 的三个磁盘映像来自同一次通用构建：单架构版本对应用包中每个多架构文件用 `lipo -thin` 只保留一种架构，再重新做 ad-hoc 签名。CI 挂载每个映像，核对签名与架构后启动应用；x86_64 版本在 Apple 芯片运行器上经 Rosetta 2 启动，运行器无法安装 Rosetta 2 时只给出警告。

## 3. 校验文件

每个 Release 都附带：

- `SHA256SUMS`：GNU coreutils 格式，每行 `<64 位小写十六进制>␠␠<产物名>`，按产物名排序，LF 换行，覆盖上表全部产物。校验：`sha256sum --ignore-missing -c SHA256SUMS`（Linux）、`shasum -a 256 --ignore-missing -c SHA256SUMS`（macOS）、`(Get-FileHash <文件>).Hash`（Windows，与对应行比较，大小写不敏感）。
- `release.json`：供脚本读取的清单，`schema_version` 为 1：

```json
{
  "schema_version": 1,
  "channel": "nightly",
  "tag": "nightly",
  "version": "7.2.10",
  "commit": "<40 位提交>",
  "date": "2026-10-02",
  "assets": [
    {
      "name": "SereinGram-linux-x86_64.AppImage",
      "os": "linux",
      "arch": "x86_64",
      "kind": "appimage",
      "size": 123456789,
      "sha256": "<64 位小写十六进制>",
      "url": "https://github.com/eltavine/SereinGram/releases/download/nightly/SereinGram-linux-x86_64.AppImage"
    }
  ]
}
```

`channel` 为 `nightly` 或 `release`；`version` 是构建所基于的 Telegram Desktop 版本；`kind` 取值 `installer`、`portable`、`disk-image`、`appimage`、`deb`、`rpm`、`flatpak`。新增字段只追加，不改变已有字段的含义。

## 4. 发布页内容

Release 正文最上方是自动生成的变更记录：Nightly 列出自上一个 Nightly 以来、正式版列出自上一个 `v*` 标签以来 `develop` 主线上的提交，按 Conventional Commits 类型分为不兼容变更、新功能、修复、性能与 Telegram Desktop 上游合并，构建、工具与文档等其余提交折叠显示；随后是按系统与架构排列的下载表与校验说明。生成逻辑在 `tools/serein/release.py`。

## 5. API 凭据

正式版必须使用 SereinGram 自己的 API 凭据（仓库 Secrets `SEREIN_API_ID`、`SEREIN_API_HASH`）。缺少凭据时 `serein-release.yml` 仍完成整套构建并保留产物供内部测试，但不发布正式版，并以“Release credentials”任务失败提示配置。Nightly 在缺少凭据时改用 Telegram Desktop 源码中公开的测试凭据（`TDESKTOP_API_TEST`）构建并照常发布，“Release credentials”任务只给出警告；`release.py notes --test-credentials` 把 `tools/serein/credentials_notice.md` 中的英文声明放在正文最上方：SereinGram 尚未配置开发者 API 凭据，本 Nightly 使用 Telegram Desktop 公开的测试凭据构建，登录可能受限或失败，配置凭据后会自动发布新的 Nightly。声明的措辞由 `tools/serein/tests/test_release.py` 固定。配置凭据后的下一个 Nightly 不再带这段声明。

## 6. 构建矩阵与质量门禁

| 工作流 | 触发 | 内容 |
| --- | --- | --- |
| `serein-guards.yml` | 每次推送与 PR | 格式与风格、Lint、静态分析、依赖与配置校验、生成代码一致性、上游侵入预算、提交信息、核心单元测试 |
| `serein-win.yml`、`serein-mac.yml`、`serein-linux.yml` | `develop` 与 `main` 推送、指向 `develop` 的 PR（Debug）；被发布流程调用（Release，Windows 与 Linux 各含 x86_64 和 arm64） | 编译（警告即错误）、`test_serein`、启动冒烟测试、打包与安装测试；Linux 发行版安装测试是单独的任务 |
| `serein-flatpak.yml`、`serein-arch.yml` | 打包文件改动、每周定时；Flatpak 也被发布流程调用 | 发行版打包与安装检查 |
| `serein-release.yml` | 每日定时、手动、`v*` 标签 | Release 矩阵、校验文件、变更记录与发布 |
| `serein-upstream.yml` | 每周一、手动 | 合并 Telegram Desktop `dev`，开同步 PR 并触发守卫与三平台构建 |

构建尽量可重复：依赖与工具按版本或校验和固定，Linux 构建以提交时间作为 `SOURCE_DATE_EPOCH`，便携包的文件顺序、属主与时间戳固定；不承诺逐字节一致。运行器镜像固定为具体版本（`ubuntu-24.04`、`ubuntu-24.04-arm`、`macos-26`、`windows-2025-vs2026`、`windows-11-arm`），不随 `-latest` 标签迁移。

外部服务的偶发故障不应让发布失败：

- 下载用 `curl --retry`；包管理器、`hdiutil`、Git 标签获取与 Release 上传用 `tools/serein/retry.sh` 按倍增的间隔重试；Linux 依赖镜像构建失败后再试一次，已完成的层不重做。
- Linux 发行版安装测试在单独的任务中下载构建产物运行，重跑它不需要重新构建。每个发行版最多三次，每次换新容器并有时间上限，openSUSE 从第二次起停用原有镜像源、改从源站 `downloadcontent.opensuse.org` 下载；安装成功后的文件或动态库检查失败不再重试。只有镜像或软件包仓库在每次尝试中都无法访问或超时，发行版才记为“无法判定”并给出警告；其余失败都让任务失败，镜像名不存在、一个发行版都没能检查也算失败。结果表逐行写入任务摘要。
- 所有 Nightly 运行共用一个并发组，依次执行。发布先把全部文件上传到草稿，核对 Release 上的文件名、大小与上传状态后才替换 `nightly`，公开后再核对一次；重跑发布任务会复用同一个草稿并更新其目标提交、标题与正文，成功后删除更早运行留下的 Nightly 草稿。
