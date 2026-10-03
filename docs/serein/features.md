# 功能矩阵

本表是 SereinGram 功能状态的唯一来源，状态与优先级定义见 [产品规格](README.md#3-标识与状态)。

来源缩写：`Ni` Nagram iOS · `Na` Nagram Android（含 NekoX／Nekogram 继承项）· `Ad` AyuGram Desktop · `Aa` AyuGram Android · `Sw` Swiftgram · `T` Telegram Desktop 已有 · `D` 桌面补充。

部分功能继承自 Nagram-qt，代码与核心测试已完成但多数缺少现场验收，统一记为 `Implemented`。

核对口径：

- AyuGram：`docs.ayugram.one/desktop` 功能列表、AyuGramDesktop 与 AyuGram4A 的 README（2026-09-30 抓取）。
- 手机版 Nagram：Nagram iOS 的集中偏好，Nagram Android 继承自 NekoX／Nekogram 的偏好、菜单动作与无开关能力，以及 Nagram Android README（2026-09-30 整理）；按下文“需求族映射”归入本表各族，排除项见末节。

## PLAT 平台与交付

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-PLAT-01 | macOS 通用二进制（arm64 + x86_64）构建与 DMG（依赖与应用按 `x86_64;arm64` 构建，CI 用 `lipo -verify_arch` 校验两种架构；发布产物为 Release 配置的 `SereinGram-macos-universal.dmg`，以及由同一构建用 `lipo -thin` 只保留一种架构的 `SereinGram-macos-arm64.dmg` 与 `SereinGram-macos-x86_64.dmg`，三个映像都在 CI 中挂载、核对架构并启动；当前阶段不使用开发者证书签名、不公证，只做不需要证书的 ad-hoc 签名，使 Apple 芯片能够启动、下载后可按住 Control 点按打开） | T | Implemented | P0 |
| SG-PLAT-02 | Windows x86_64 构建、安装包与便携版（CI 每次构建都以 Inno Setup 编译 `setup.iss`，失败即构建失败；发布产物为 Release 配置的 `SereinGram-windows-x86_64-setup.exe` 与 `SereinGram-windows-x86_64-portable.zip`；当前阶段不签名） | T | Implemented | P0 |
| SG-PLAT-03 | Linux x86_64 静态构建（Rocky Linux 8 容器，glibc 2.28 起）与便携包（发布产物为 Release 配置的 `SereinGram-linux-x86_64.tar.xz` 与 `SereinGram-linux-x86_64.AppImage`，AppImage 用固定版本与校验和的 appimagetool 1.9.1 和 type2 运行时生成；便携包以提交时间为 `SOURCE_DATE_EPOCH`，文件顺序、属主与时间戳固定；PR 构建保留一份编译缓存） | T | Implemented | P0 |
| SG-PLAT-04 | 三平台 CI 与质量门禁（`serein-{mac,win,linux}.yml` 在 PR 中构建应用（警告即错误）、运行 `test_serein` 与启动冒烟测试并打包；`serein-guards.yml` 在每次推送与 PR 运行格式与风格检查、ruff、shellcheck、yamllint、markdownlint、`buf lint` 与 `buf format`、clang-tidy 静态分析、actionlint 与 zizmor 工作流检查、桌面入口与 AppStream 校验、打包依赖版本、模块边界、上游侵入预算、生成代码一致性、核心单元测试，以及 commitlint 与 gitleaks；任何一项失败都会让对应任务失败） | D | Implemented | P0 |
| SG-PLAT-05 | 发行版打包：Flatpak 清单、AUR PKGBUILD、`DESKTOP_APP_USE_PACKAGED` 依赖清单（Arch Linux：`packaging/arch/PKGBUILD` 以系统库构建 `sereingram-desktop-git`，依赖清单与 Arch 官方 telegram-desktop 一致，tde2e 按 `snap/snapcraft.yaml` 锁定的 tdlib 提交现场编译，守卫 `check_packaging.py` 要求 PKGBUILD 与 Flatpak 清单锁定的 tdlib、tg_owt、tlottie、patches 提交与 Qt 版本都与 snap 配方一致；打包者通过 `SEREIN_API_ID` 与 `SEREIN_API_HASH` 提供自己的凭据，缺少时构建直接报错，绝不使用官方 Telegram 凭据；CI 工作流 `serein-arch.yml` 在 Arch 容器中用 makepkg 构建当前提交、安装后检查文件与动态库并上传包，已在 CI 中构建并安装通过；以系统库构建时不启动 GitHub 更新检查，设置页改为说明由系统包管理器更新；Flatpak：`packaging/flatpak/io.github.eltavine.SereinGram.yml` 基于 GNOME 51 运行时，依赖模块与 Flathub 的 Telegram 清单一致，构建本地检出，凭据来自被忽略的 `api_credentials.local.cmake`；CI 工作流 `serein-flatpak.yml` 构建并上传 `.flatpak` 包，已在 CI 中生成；尚未提交到 Flathub；Debian／Ubuntu 与 Fedora／openSUSE：Linux 工作流用 nFPM（`packaging/nfpm/`）把 CentOS 基线构建的同一个程序连同桌面入口、图标与 AppStream 元数据打成 `.deb` 与 `.rpm`，与 AppImage 一起上传，覆盖 glibc 2.28 及以上的主流发行版） | Ad | Implemented | P2 |
| SG-PLAT-06 | Windows arm64 构建（发布流程以 arm64 参数调用 Windows 工作流，在原生 arm64 运行器上不读写缓存、从头构建 Debug 与 Release 依赖、Qt 与应用，生成 `SereinGram-windows-arm64-setup.exe` 与 `-portable.zip`；依赖与 Qt 在 arm64 上已从头构建成功；合并了 Serein 文案后，生成的语言键查找函数超出 MSVC arm64 的函数大小限制（C1053），构建时由 `tools/serein/split_lang_keys.py` 按键名首字母拆成多个函数，查找结果不变，待 arm64 应用构建通过） | T | In Progress | P2 |
| SG-PLAT-07 | 独立更新检查与正式版发布（界面设置“在 GitHub 上检查更新”默认开启：启动 30 秒后及每 24 小时检查一次：发布流程构建的 Nightly 带有渠道与构建提交（CMake 把它们写进单独生成的源文件，新提交不会让其他编译结果失效），检查滚动的 `nightly` 预发布，目标提交与自身不同即提示新的 Nightly；其他构建查询最新正式版；只接受 github.com 的发布页链接，发现新版本时提示链接，不自动下载；官方更新通道在构建中关闭；推送 `v*` 标签时 `serein-release.yml` 构建 Release 矩阵，生成变更记录、`SHA256SUMS` 与 `release.json`，创建草稿 Release，带后缀的标签标为预发布，核对后手动发布） | D | In Progress | P2 |
| SG-PLAT-08 | API 凭据构建期注入（Secrets 或本地文件），禁止使用官方客户端凭据 | D | Implemented | P0 |
| SG-PLAT-09 | Nightly 构建与发布（`serein-release.yml` 每天 19:00 UTC 从 `develop` 最新提交构建 Windows x86_64／arm64、macOS universal／arm64／x86_64、Linux x86_64／arm64 与 Flatpak x86_64／arm64 的 Release 产物，分支未变化时跳过；滚动的 `nightly` 预发布先以草稿上传再一次替换；正文顶部是自上一个 Nightly 以来按类型分组的变更记录，下方是下载表；附 GNU 格式的 `SHA256SUMS` 与 `release.json` 清单；产物名称固定，发布前与 `tools/serein/policy/release_assets.json` 逐一核对；约定见 [releases.md](releases.md)；默认分支为 `develop`，定时触发直接使用其中的工作流；正式版需要仓库 Secrets 中的 SereinGram API 凭据，缺少时只构建不发布；Nightly 在缺少凭据时用上游公开的测试凭据构建并照常发布，正文最上方以英文声明尚未配置开发者凭据、登录可能受限或失败，配置后自动消失；上传后先核对文件名与大小再替换，外部服务的偶发故障按 [releases.md](releases.md) 重试） | D | In Progress | P1 |
| SG-PLAT-10 | Linux arm64 构建（Linux 工作流以 arm64 参数在原生 arm64 运行器上用同一个 Rocky Linux 8 容器构建，生成 `SereinGram-linux-arm64` 的便携包、AppImage、`.deb` 与 `.rpm`，并运行启动冒烟测试、AppImage 与软件包安装测试；发布流程同时构建 x86_64 与 arm64；Flatpak 清单另在 arm64 运行器上生成 `SereinGram-linux-arm64.flatpak`；arm64 用 `-fsigned-char` 与其他目标保持一致的 `char` 符号） | T | Implemented | P2 |
| SG-PLAT-11 | NixOS 与 Nix（根目录 `flake.nix` 导出 `packages`、`overlays.default` 与使用测试凭据的 `checks`，支持 x86_64 与 aarch64；`packaging/nix/package.nix` 在 nixpkgs 的 telegram-desktop 之上替换源码，剔除其中的官方凭据参数，按 `apiId`/`apiHash` 传入打包者自己的凭据，缺少时构建报错，`testCredentials` 仅供自动化检查；flake 声明需要子模块，要求 Nix 2.27 起；CI 工作流 `serein-nix.yml` 检查 nixfmt 格式，从源码构建并在 Xvfb 中启动） | D | In Progress | P2 |
| SG-PLAT-12 | 七个主流 Linux 发行版的原生安装与 CI 验证（Ubuntu、Debian、Linux Mint 用 `.deb`，Fedora、openSUSE 用 `.rpm`，Arch Linux 用 PKGBUILD 或便携版，NixOS 用 flake；`tools/serein/linux_package_test.sh` 在 Debian 12/13、Ubuntu 22.04/24.04/26.04、Linux Mint 22.3、Fedora 43、openSUSE Tumbleweed 与 Leap 16.1 的容器中用系统包管理器安装并检查文件与动态库，在 Arch 容器中检查便携版，作为单独的任务每个发行版最多试三次，镜像或软件包仓库始终无法访问或超时则记为无法判定；支持 Snap 的发行版另有 core24 Snap 包，`serein-snap.yml` 构建、安装并启动，配方中的上游官方凭据已删除，`check_packaging.py` 拒绝任何已提交配方写入凭据；各发行版的安装命令见 [Linux 发行版](linux.md)） | D | In Progress | P1 |

## BRAND 品牌与身份

Nagram 的品牌政策要求分支使用不同品牌并替换 Nagram 名称与图标，以下各项是发布前提。

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-BRAND-01 | 应用名 SereinGram：可执行文件、关于页、托盘、通知、安装器文案 | D | Implemented | P0 |
| SG-BRAND-02 | 自有图标资产，替换全部 Nagram 图标（正式图标由 [OukaroMF](https://github.com/OukaroMF/) 设计，母版 `Telegram/Resources/branding/sereingram-logo.svg` 原样保存；`tools/serein/brand/generate_icons.py` 用 resvg 渲染母版，生成浅色圆角底板上的全部应用图标、Windows ICO、macOS 图标集、圆形与绿色变体，以及复用母版深色路径的单色托盘、设置入口、二维码登录与 symbolic 图标） | D | Implemented | P0 |
| SG-BRAND-03 | 应用 ID、数据目录、便携目录、Windows AppUserModelID、安装器 ID 与通知激活器 GUID | D | Implemented | P0 |
| SG-BRAND-04 | 源码、发布与问题反馈链接指向 `eltavine/SereinGram` | D | Implemented | P0 |
| SG-BRAND-05 | 代码更名：`nagram` → `serein`（目录、命名空间、文案键、存储键、CMake、测试目标、工作流） | D | Implemented | P0 |
| SG-BRAND-06 | 第三方许可声明（SereinGram 设置首页“第三方许可”：列出随程序分发的 OpenCC、marisa-trie、darts-clone、RapidJSON、quirc 与注册日期数据的名称、许可与地址，许可全文作为 Qt 资源打包，可在程序内查看；文本直接取自子模块中的许可文件，RapidJSON 的子集不含许可文件，以其头文件中的版权声明补齐 MIT 文本） | D | Implemented | P1 |

## CORE 基础设施

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-CORE-01 | proto3 schema 作为设置、结构化配置、导出包与历史记录的唯一声明来源；Buf lint 与 breaking 检查（13 个设置 proto 生成 195 个选项，另有配置、导出格式与历史记录 proto；菜单配置、重复发送确认、消息过滤、链接规则、消息截图、翻译与转写服务、“优先系统 AI”均由设置 proto 以自定义界面字段声明，`buf breaking` 可发现键名改动；仍在 C++ 中手写的只剩本地别名 1 个选项：它的存储校验要把每个键解析成应用层的对话 ID 并检查类型，核心测试无法链接，改用纯函数校验会让含非法键的导入被接受、再在使用时整体失效） | D | Implemented | P0 |
| SG-CORE-02 | 代码生成器：注册表元数据、C++ 值类型、JSON 编解码、校验（均已生成；既有结构化配置已迁移 10 个：链接规则、快捷回复、消息过滤、主菜单、消息菜单、翻译与转写服务、历史排除列表、本地别名、贴纸目录导出文件、消息截图设置；所有存储与导出格式都由 proto 声明；过滤、链接规则、主菜单与翻译转写服务的设置界面均已改用生成的结构体读写，JSON 只保留在编解码与校验层；支持字符串键的 map 字段、double 与可空字段） | D | Implemented | P0 |
| SG-CORE-03 | 存储端口与适配器：设备偏好、账号偏好、历史库（选项存储分设备与账号两个作用域；历史库经 `Ports::HistoryStore` 端口由 Qt SQL 适配器实现，记录整体用基于本地密钥派生的 AES-GCM 加密，缓存媒体副本用同一密钥加密） | D | Implemented | P0 |
| SG-CORE-04 | 上游挂钩门面 `serein/hooks`，上游文件只调用门面（设置选项的门面由 proto 生成到 `serein/hooks/gen`，面向上游的薄接口头文件已移入 `serein/hooks/<领域>/`，并可脱离应用代码单独通过语法检查；直接包含内部头文件的上游文件已从 87 降到 0，并由预算守卫锁定；依赖上游嵌套类型的门面声明为函数模板，在实现文件中对该类型显式实例化，门面本身不包含应用代码） | D | Implemented | P0 |
| SG-CORE-05 | 功能模块注册与生命周期（应用、会话、窗口作用域）：应用与会话作用域已由 `serein/app/modules.cpp` 模块表统一分发（对话排序已从上游窗口构造函数移到会话作用域）；窗口作用域由 `Serein::Hooks::OnWindowStarted` 分发，首个使用者是最近会话记录；主菜单条目统一由 `Serein::Hooks::FillMainMenu` 添加 | D | Implemented | P0 |
| SG-CORE-06 | 设置页与搜索索引由 schema 元数据生成（开关、选项、数值、文本、子页）：开关行、小标题、说明、依赖开关与自定义行位置已由 proto 生成 `AddLayout`，界面、聊天、消息、写作、菜单、媒体、隐私（含幽灵与历史）与服务八页已迁移（服务页的普通开关与说明由生成器产生，来源选择、系统 AI、服务列表、代理工具与 DoH 为自定义行）；数值输入行（范围取自 gte/lte 规则，0 的文字与复数格式由 `number` 选项声明）已生成；单选行（按序语言键或 `in` 规则加后缀）已生成；文本行（`text` 选项声明占位语言键，未设置时行标签显示占位文字，`disabled_by` 使该行在对应开关打开时隐藏）已生成，“已编辑/已删除标记文字”已改用生成行；子页由页面选项 `subpage` 声明标题、图标与搜索关键词，生成父页上的入口按钮 `AddSubpageButton` 以及分区标题、图标常量，分区类本身沿用各页相同的手写样板；幽灵模式与“已删除与已编辑消息”已拆成隐私页下的子页 | D | Implemented | P0 |
| SG-CORE-07 | 英文、简体、繁体内置文案与一致性检查 | Ni Na | Implemented | P0 |
| SG-CORE-08 | 配置管理：已修改项、导出、导入差异预览、诊断信息（J01–J04）；“备份到收藏夹”把本设备设置的导出文件 `sereingram-settings.json` 带固定标签发送到收藏夹，“从收藏夹恢复”按标签搜索收藏夹中的文档、取最新的同名备份下载后进入同一个导入差异预览，换设备时不必重新配置，系统凭据库中的密钥不在其中；“恢复默认设置”先列出本设备上所有已修改的可导出设置及其当前值，确认后一次恢复为默认值，按账号保存的设置不受影响 | Ni Na | Implemented | P1 |
| SG-CORE-09 | 守卫：源文件 ≤ 1000 行、模块依赖方向、上游侵入预算、生成代码漂移 | D | Implemented | P0 |
| SG-CORE-10 | 测试：纯逻辑单元测试与 `-testagent` 界面场景（核心测试基于 doctest，每个测试文件用 `TEST_CASE` 自注册，用例单独报告并可按名称筛选） | D | Implemented | P0 |
| SG-CORE-11 | 更多界面语言的社区翻译平台接入（Serein 字符串按界面语言加载任意 `langs/serein/<语言代码>.strings`，缺失的键回退英文，CMake 扫描目录自动生成资源清单；核心测试要求中文译文完整、其他译文的键与占位符与英文一致；`crowdin.yml` 与同步工作流 `serein-crowdin.yml` 已就绪：在 GitHub Secrets 中配置 Crowdin 项目 ID 与访问令牌后，英文源文件改动时上传，每周下载译文并向 develop 提交 PR） | Ad Na | Implemented | P3 |

## GHOST 幽灵模式

策略模型与按账号保存的设置（`proto/serein/settings/v1/ghost.proto`、`serein/features/ghost/model/`）已实现并有测试；上游挂钩与设置界面待接入。

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-GHOST-01 | 不发送消息已读回执（私聊、群组、频道、讨论、话题、提及与反应已读）（对话、话题与评论、频道私信子列表与内容已读均已拦截，本地已读状态照常推进；“全部提及已读”“全部回应已读”只清除自己的计数，不拦截） | Ad Aa Na | Implemented | P1 |
| SG-GHOST-02 | 不发送动态已读与动态浏览 | Ad Aa Na | Implemented | P1 |
| SG-GHOST-03 | 不发送在线状态；发送消息后立即恢复离线 | Ad Aa Na | Implemented | P1 |
| SG-GHOST-04 | 不发送输入、上传、选贴纸等活动状态 | Ad Aa Na | Implemented | P1 |
| SG-GHOST-05 | 总开关、子项锁定，全局策略与按账号策略（各账号的总开关与子项按账号保存，子项在总开关关闭时不生效；设备级“所有账号启用幽灵模式”默认关闭，开启后每个账号都按各自子项进入幽灵模式；主菜单开关与快捷键显示实际生效状态，关闭时同时关闭账号与全局开关） | Ad Na | Implemented | P1 |
| SG-GHOST-06 | 主菜单、托盘与聊天顶部的快速切换入口和状态指示（界面设置“主菜单与托盘菜单中的 SereinGram 快捷入口”默认关闭，开启后主菜单开关显示当前账号的实际状态，托盘菜单切换“所有账号启用幽灵模式”，文字随状态变化，托盘菜单在重启后更新；快捷键可切换；幽灵模式生效时聊天顶栏的状态文字前显示 👻，钩子位于上游计算状态文字之后，先去掉旧前缀再按当前状态添加，状态文字在下次刷新时更新） | Ad Na | Implemented | P1 |
| SG-GHOST-07 | 阅读频道消息时不增加浏览数 | Ad | Implemented | P2 |
| SG-GHOST-08 | 发送消息或互动后自动标记该对话已读（可选） | Ad Na | Implemented | P2 |
| SG-GHOST-09 | 幽灵模式下用定时消息发送，避免上线（在 `Api::SendAction` 构造处统一挂钩：未手动定时、非快捷消息、非收藏夹时改为 12 秒后定时发送） | Ad | Implemented | P2 |
| SG-GHOST-10 | “仅本地已读”与“同步到服务端”两个显式动作：读到此处、全部已读（消息菜单“读到此处”已实现：幽灵模式隐藏已读时，把已读同步到该条为止；对话列表的“标记为已读”“全部标记为已读”默认只在本地生效，开启“手动标记已读时发送已读回执”后同步到服务端） | Ad Na | Implemented | P2 |
| SG-GHOST-11 | 查看动态前提示当前幽灵模式状态（“不标记动态为已看”生效时，每个账号在本次运行中第一次观看动态会弹出提示，说明这次观看不会被标记为已看） | Na | Implemented | P3 |
| SG-GHOST-12 | 幽灵模式下静音发送（幽灵模式子项“静音发送消息”按账号保存，默认关闭；幽灵模式生效时发出的消息不触发对方的通知提示音，与“默认静音发送”任一开启即静音） | Aa | Implemented | P3 |

## HIST 消息历史与防撤回

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-HIST-01 | 保存已接收的删除消息并在原位显示，附删除标记（防撤回）：本次会话内原位保留与底部“已删除”标记已实现（默认关闭）；重启后每次加载消息分片时，从历史库取出该范围内的删除记录，以本地消息按时间插回原位（文字与格式保留，媒体显示为摘要文字，不含反应与回复关系）；回复所引用的消息已被删除且历史库中有记录时，引用处显示“已删除的消息：”加原文的前 64 个字符，没有正文时显示媒体摘要 | Ad Aa Na | Implemented | P1 |
| SG-HIST-02 | 保存编辑历史，在消息菜单查看各版本 | Ad Aa Na | Implemented | P1 |
| SG-HIST-03 | 历史库加密存储（账号本地密钥派生），退出账号时清理（AES-256-GCM 密钥由账号本地密钥派生；退出账号后同一账号槽位的本地密钥会保留，因此在会话结束且不是退出应用时删除历史库及其日志文件） | D | Implemented | P1 |
| SG-HIST-04 | 保留期限、容量上限、单对话清理与全部清理（保留天数与条数上限有设置行，会话启动时清理；“已删除消息”窗口可清除单个对话的记录，隐私页“清除已保存的消息历史”清除整个账号，两者都需确认） | Na | Implemented | P1 |
| SG-HIST-05 | 自定义已删除、已编辑标记文字；已删除消息半透明（消息页“标记与计数”下的“已编辑标记文字”“已删除标记文字”已实现，最长 64 字符；“淡化已删除消息”默认开启，原位保留的已删除消息以 60% 不透明度绘制） | Aa Na | Implemented | P2 |
| SG-HIST-06 | 按对话浏览已删除消息（对话菜单与消息菜单的“已删除消息”列出该对话最近 100 条，含发送者与发送时间；另存的缓存图片在列表中显示缩略图，点击用系统程序打开；列表顶部“以聊天形式查看”用上游的 `FakeHistoryItem` 临时消息按时间顺序画成聊天气泡，使用当前聊天主题背景、跨天显示日期、群聊显示头像与发送者，媒体以摘要文字显示；与“在原位置保留已删除消息”共用同一套还原逻辑） | Ad Aa | Implemented | P2 |
| SG-HIST-07 | 是否记录机器人消息；按对话排除（“记录机器人消息”为隐私页开关；消息菜单“不记录此对话的历史”按账号维护排除列表，列表格式由 `config/v1/history_exclusions.proto` 声明） | Na | Implemented | P2 |
| SG-HIST-08 | 已下载媒体不随删除清除，历史中可继续打开（删除时记录已下载到磁盘的文件路径，“已删除消息”中文件仍在时显示“打开文件”；未下载、但已完整载入内存的媒体（看过的图片、语音、贴纸与小文件，单个不超过 20 MB）用历史库的密钥加密另存到账号目录的 `serein_media`，“打开媒体”解密到临时目录后交给系统程序打开；清除对话、退出登录与过期清理时一并删除，流式播放未完整下载的视频不在此列） | Ad Aa | Implemented | P2 |
| SG-HIST-09 | 限时图片、视频过期后仍可查看（隐私页“保留已过期的限时媒体”：到期时不清除缓存、不替换为“已过期”，本次会话内可反复打开；重启后按服务端状态显示。自动删除计时到期的消息按“原位保留已删除消息”处理并写入历史库） | Ad Aa | Implemented | P2 |
| SG-HIST-10 | 保留被移出或封禁的群组、频道的本地记录（历史设置“保留被移出的对话的消息”，默认关闭且需要“保留已删除消息”：频道或超级群变为不可访问时，把本设备已加载的最近 500 条消息按已删除消息存入本地历史；历史子页“浏览已保存的对话”列出有已删除记录的对话并打开对应的“已删除消息”，被移出后对话列表中已看不到的对话也能查看；基础群被移出时上游不发出可靠的更新，暂不覆盖） | Aa | Implemented | P3 |

## FILTER 过滤与规则

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-FILTER-01 | 正则过滤：遮盖、替换、隐藏，排除对话，规则测试框，导入导出（I01） | Ad Aa Ni Na | Implemented | P1 |
| SG-FILTER-02 | 隐藏已屏蔽用户的消息（I01 子项） | Ad Na | Implemented | P1 |
| SG-FILTER-03 | 在回复引用、反应列表、成员列表中也隐藏已屏蔽用户（回复引用被过滤的消息时显示“已隐藏”占位；开启“隐藏已屏蔽用户”后，“谁已读/谁回应”列表不列出已屏蔽用户，服务端给出的总数不变；成员列表保持完整，便于管理员管理） | Ad | Implemented | P2 |
| SG-FILTER-04 | 按对话的过滤规则与共享规则列表导入（规则可限定在若干对话 ID 内，留空对所有对话生效；剪贴板规则列表的格式由 `FilterRuleList` 声明，导入时追加为停用的新规则并需确认，导出时去掉对话范围；过滤配置升级到 v2 并自动迁移 v1） | Ad | Implemented | P2 |
| SG-FILTER-05 | Zalgo 字符过滤 | Ni | Implemented | P2 |
| SG-FILTER-06 | 消息菜单“隐藏此人的消息”（E23） | Ni | Implemented | P2 |
| SG-FILTER-07 | 链接规则：URL 修正、参数清理、打开前确认（I02） | Ni | Implemented | P2 |
| SG-FILTER-08 | 全局、账号、对话、话题四级规则继承与远程规则源（全局：设备级“规则订阅”从 HTTPS 地址下载“导出”格式的规则列表，每天更新并可立即更新，对所有账号生效；账号：账号自己的过滤规则；对话与话题：规则可限定在若干“对话 ID”或“对话 ID:话题 ID”内，对话级规则覆盖该对话的所有话题；四级在过滤开启时依次生效，账号规则先于全局订阅规则） | Ni | Implemented | P3 |
| SG-FILTER-09 | 在本地隐藏单条消息（消息菜单“隐藏消息”，默认隐藏，可在菜单设置中显示；隐藏后该消息或整组相册在本设备上显示为“此消息已在本设备上隐藏”，回复中的引用同样替换，菜单项变为“显示消息”；按账号保存，最多 1000 条，超出时丢弃最早的；过滤规则窗口中可一次全部显示） | Aa | Implemented | P2 |
| SG-FILTER-10 | 用链接规则改善链接预览（链接规则窗口“将链接规则用于链接预览”，默认关闭；开启后编写消息时按启用的规则改写链接再向服务器请求预览，例如把网站指向能生成完整预览的镜像，发送时附上该预览，消息文字仍保留输入的链接） | Ad Na | Implemented | P2 |
| SG-FILTER-11 | 在当前对话中临时显示被过滤的消息（启用过滤规则后，对话菜单出现“显示被过滤的消息”，只在本次运行中对该对话生效，再次选择“隐藏被过滤的消息”恢复；在本地手动隐藏的消息不受影响） | Aa | Implemented | P3 |

## PRIV 隐私与本地能力

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-PRIV-01 | 主播模式：截屏录屏排除窗口，遮盖会话列表、标题与通知（G02） | Ad Ni | Implemented | P1 |
| SG-PRIV-02 | 主播模式快捷键与菜单、托盘入口（主菜单开关与托盘菜单“开启/关闭演示模式”已接入，随界面设置“主菜单与托盘菜单中的 SereinGram 快捷入口”出现，该设置默认关闭；托盘项由 `serein/app/tray_menu.cpp` 提供；快捷键命令 `serein_toggle_presentation_mode` 与 `serein_toggle_ghost_mode` 默认不绑定按键，可在 `shortcuts-custom.json` 中绑定，切换后提示当前状态） | Ad | Implemented | P1 |
| SG-PRIV-03 | 遮盖本机手机号（G01） | Ni Na | Implemented | P1 |
| SG-PRIV-04 | 本地备注名称（隐私设置“对话本地名称”默认关闭；开启后资料菜单出现“设置本地名称”，本地名称替换用户、群组与频道在本设备上的显示；关闭时保留已保存的名称但不显示，切换时刷新已加载对象的名称） | Ni | Implemented | P2 |
| SG-PRIV-05 | 默认隐藏赞助消息与代理赞助频道（B10、B11） | Ad Ni | Implemented | P1 |
| SG-PRIV-06 | 隐藏已读时间提示与分享手机号提示（G03、G04） | Ni | Implemented | P2 |
| SG-PRIV-08 | 受保护内容的本地复制与保存（复制文字、保存媒体与截图已放开，转发仍由服务端拒绝；受保护的动态可截图，并与普通动态一样可由 Premium 用户保存，不绕过 Premium 限制） | Ad Na | Implemented | P2 |
| SG-PRIV-09 | 设置锁（隐私设置“用本地密码锁定 SereinGram 设置”默认关闭，设置了上游本地密码时，打开任一 SereinGram 设置页（包括从设置搜索直达的子页）先显示密码输入，用上游 `checkPasscode` 校验，每次启动及应用被锁定后重新上锁；本地隐藏已登录账号列入明确排除） | Ni | Implemented | P3 |
| SG-PRIV-10 | 检测到录屏软件时自动开启主播模式（隐私设置“运行直播或录屏软件时自动开启”，默认关闭：开启后每 5 秒在后台线程列出进程——Windows 用 Toolhelp 快照、macOS 用 libproc、其他平台读 `/proc/*/comm`——发现 OBS、Streamlabs、XSplit、vMix、Bandicam 等常见软件即打开主播模式，软件退出后恢复原状，期间手动改过则不再动它） | Ad | Implemented | P3 |
| SG-PRIV-11 | 设备详情中的登录时间、API ID 与官方应用标记（隐私设置“显示更多设备信息”，默认关闭：开启后“设置 > 设备”中每个会话的详情在系统版本之后加入服务端返回的登录时间、API ID 以及是否为官方应用，便于识别第三方客户端登录的会话；数据来自上游已请求的会话列表，不额外发送请求） | Ad | Implemented | P3 |
| SG-PRIV-12 | 扫描二维码与扫码登录（隐私设置“扫描二维码”：从全部屏幕截图、剪贴板图片或图片文件中识别二维码，解码使用固定版本的子模块 quirc 1.2（ISC 许可）；内容为 `tg://login?token=` 时先以警示样式确认“只扫描自己设备上的二维码”，再调用 `auth.acceptLoginToken` 让显示二维码的设备登录本账号，提示登录的应用与设备，二维码过期或已使用时给出说明；其他内容以文字列出，`tg://` 与网页链接经上游的隐藏链接确认后打开；macOS 识别屏幕需要屏幕录制权限，Wayland 下屏幕截图不可用时可改用剪贴板或文件；识别逻辑只接收灰度像素，核心测试用仓库内的二维码编码器生成登录二维码再识别，三个平台的 `test_serein` 都运行） | Na | Implemented | P3 |

## APPEAR 界面与外观

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-APPEAR-01 | 气泡与头像圆角（A02–A04） | Ad Ni | Implemented | P1 |
| SG-APPEAR-02 | 消息宽度、频道宽消息、隐藏气泡尾巴、引用配色、回复缩略图、忽略对话主题（A05–A10） | Ad Ni | Implemented | P1 |
| SG-APPEAR-03 | 主菜单标题、顺序、显隐与节日装饰（A11） | Ni | Implemented | P1 |
| SG-APPEAR-04 | 应用图标角标、通知延迟（A12–A14） | Ni | Implemented | P2 |
| SG-APPEAR-05 | 界面半角标点（A15） | Ni | Implemented | P2 |
| SG-APPEAR-06 | 主字体自定义（由上游聊天设置中的字体选择提供，即 `customFontFamily`；自定义等宽字体列入明确排除） | Ad Ni | Implemented | P2 |
| SG-APPEAR-07 | 应用图标选择（界面设置“应用图标”：选择至少 64×64 的 PNG、JPEG 或 WebP 图片，缩到 512 像素以内存为 `tdata/serein_app_icon.png`，启动时经上游 `Window::OverrideApplicationIcon` 替换窗口与任务栏图标，macOS 另经 `base::SetCustomAppIcon` 替换程序坞与访达图标；可恢复默认；内置图标方案待有正式图标素材后再加） | Ad Ni | Implemented | P2 |
| SG-APPEAR-08 | 以频道身份发言时显示频道徽标（消息页开关，默认关闭：超级群中以广播频道身份发送的消息在名字右侧显示上游已有的“频道”标记；匿名管理员以群身份发言时不显示） | Ad Ni | Implemented | P2 |
| SG-APPEAR-09 | 连续的文字、贴纸、圆形视频消息组的气泡尾巴修正（由上游覆盖：上游区分逻辑上的连续与气泡上的连续（`BubbleAttachedToNext`／`BubbleAttachedToPrevious`），下一条是贴纸、圆形视频等无气泡消息时文字气泡保留尾巴与大圆角，反之亦然；2026-10-01 对照 AyuGramDesktop 当前源码，相关的 `setAttachToNext` 与 `countMessageRounding` 与上游一致） | Ad | Implemented | P3 |
| SG-APPEAR-10 | 圆角贴纸（媒体设置“贴纸圆角”默认关闭；开启后聊天中的静态贴纸以大圆角生成图像并随图像缓存，动画与视频贴纸逐帧加圆角，大号表情与自定义表情不处理；钩子位于上游贴纸的图像生成与逐帧绘制） | Ad | Implemented | P3 |
| SG-APPEAR-12 | 通知位置新增顶部居中（界面设置“通知在屏幕顶部居中显示”，默认关闭；在上游通知位置选择顶部角落且使用应用内通知时，通知与“全部隐藏”按钮水平居中，向下堆叠；钩子位于上游唯一的起始位置计算函数，不改上游位置枚举与其存储） | Ad | Implemented | P3 |
| SG-APPEAR-13 | macOS 触感反馈、Force Touch 媒体预览与快速反应（触感反馈由上游覆盖：滑动回复、上拉进入下一个频道、媒体查看器与编辑器绘图使用 `base::Platform::Haptic()`；双击消息快速回应由上游提供；媒体设置“用力点按预览媒体（macOS）”默认关闭，开启后在聊天中用力点按贴纸、GIF 或图片时以上游的媒体预览显示并触发触感反馈，松开关闭，且不会再打开该媒体；钩子为主聊天消息列表与话题、回复、定时、置顶等分区消息列表构造处各一行安装，对话列表的聊天预览不接入） | Ad | Implemented | P3 |

## CHATS 会话列表与导航

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-CHATS-01 | 紧凑列表、预览行数、隐藏预览、隐藏动态（B01–B04） | Ni Na | Implemented | P1 |
| SG-CHATS-02 | 启动文件夹、隐藏“全部会话”、文件夹归档入口、隐藏文件夹未读数（B05–B08） | Ad Ni | Implemented | P1 |
| SG-CHATS-03 | 会话排序规则（B09） | Na | Implemented | P2 |
| SG-CHATS-04 | 隐藏 Premium 推广与生日提示（B12、B13） | Ni | Implemented | P2 |
| SG-CHATS-05 | 滚动到底不切换频道或话题（B14、B15） | Ni | Implemented | P2 |
| SG-CHATS-06 | 文件夹属性“仅显示我管理的群组和频道”（聊天设置“按我管理的对话筛选文件夹”默认关闭；开启后文件夹菜单出现该项，关闭时保留已标记的文件夹但不筛选，切换需重启） | Ni | Implemented | P2 |
| SG-CHATS-07 | 一键已读全部对话或当前文件夹（由上游提供：对话列表与文件夹菜单的“标记为已读”“全部对话标记为已读”，`menu/menu_mark_as_read.cpp`，核对于 2026-09-30） | Ad Na | Implemented | P2 |
| SG-CHATS-08 | 跳到对话开头（聊天窗口菜单“跳到对话开头”，由 `serein/app/peer_menu.cpp` 提供；话题内跳到话题的首条消息） | Ad Ni | Implemented | P2 |
| SG-CHATS-09 | 最近会话列表（按账号记录最近打开的 30 个对话；主菜单“最近会话”与快捷键命令 `serein_recent_chats` 打开列表，点击进入对话，可清空） | Ni | Implemented | P2 |
| SG-CHATS-10 | 聊天顶部工具栏：搜索、媒体、置顶、跳到开头、静音、清缓存（以对话菜单适配，不改上游顶栏：聊天设置“对话菜单中的快捷操作”默认关闭，开启后加入“跳到开头”、在对话中搜索、共享媒体与置顶消息列表；静音由上游对话菜单的静音子菜单提供；快捷操作中的“清除媒体缓存”确认后移除当前已加载消息（含链接预览）中图片、视频、文件与语音消息的本地缓存，包括流式播放的分片，贴纸与已保存的文件保留；本地缓存不按对话索引，未加载的较早消息不在清除范围内） | Ni | Implemented | P2 |
| SG-CHATS-11 | 保存并恢复阅读位置（聊天设置“记住阅读位置”，默认关闭：离开对话时记下顶部可见的服务器消息，滚到底部则清除，按账号保存最近 100 个对话；重启后第一次打开没有未读消息的对话时从记下的位置打开，已有本次运行的滚动状态或有未读时照常） | Ni | Implemented | P2 |
| SG-CHATS-12 | 本地置顶扩展（聊天设置“本地置顶”，默认关闭：对话菜单“本地置顶/取消本地置顶”，按账号最多 50 个；经对话排序挂钩排在服务器置顶对话之下、其余对话之上，只影响本设备的列表顺序） | Ni | Implemented | P3 |
| SG-CHATS-13 | 只搜索已有的对话（会话设置“只搜索已有的对话”，默认关闭；开启后对话列表搜索不再把输入内容发送给服务器查找公开的群组、频道与用户，也不请求搜索结果中的赞助频道，只显示已有对话；消息搜索不受影响） | Na | Implemented | P3 |

## MSG 消息显示与资料

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-MSG-01 | 时间显示秒、转发原始时间、服务消息时间、消息 ID（C01–C04） | Ad Ni | Implemented | P1 |
| SG-MSG-02 | 精确计数、隐藏浏览数、频道签名、已编辑标记（C05–C09） | Ni | Implemented | P1 |
| SG-MSG-03 | 反应显隐，按私聊、群组、频道细分（C10–C15） | Ni | Implemented | P1 |
| SG-MSG-04 | 关闭 Premium 贴纸、表情互动与消息特效（C16–C18） | Ni | Implemented | P2 |
| SG-MSG-05 | 直接显示剧透、快速转发按钮、推荐频道、会员标识、收藏夹标签栏、对方输入状态（C19–C24） | Ad Ni | Implemented | P1 |
| SG-MSG-06 | 阅读时中西文加空格与简繁转换（C25、C26） | Na | Implemented | P2 |
| SG-MSG-07 | 资料页 ID、数据中心、隐藏礼物、隐藏待办入口（G05–G08） | Ad Ni Na | Implemented | P1 |
| SG-MSG-08 | 消息详情：日期、转发来源、贴纸包与表情包作者（消息菜单“消息详情”：消息与对话、发送者 ID（Bot API 格式）、带秒的发送与编辑时间、转发来源与原始时间、浏览数与转发次数（频道消息由服务端提供）、贴纸包名称与链接、贴纸包作者） | Ad Na | Implemented | P2 |
| SG-MSG-09 | 内联按钮回调数据查看与复制（消息菜单“按钮数据”：列出内联按钮的文字与数据，点击复制；不可打印的数据以 base64 显示；默认隐藏，可在菜单设置中开启） | Ad | Implemented | P2 |
| SG-MSG-10 | 语音与圆形视频拖动进度（先核对上游现状）（由上游提供：语音消息 `VoiceSeekClickHandler` 与圆形视频 `VideoMessageSeek` 均支持拖动进度，核对于 2026-09-30） | Ad | Implemented | P2 |
| SG-MSG-11 | 反应时间显示秒（反应与已读列表的时间随“消息时间显示秒”选项显示到秒） | Ad | Implemented | P3 |
| SG-MSG-12 | 注册日期估算（标注估算来源）、波斯日历（隐私页“显示估算的注册时间”，默认关闭：按用户 ID 在 WizardLoop/CreationDate（MIT，提交 f37728802d36，许可证随 `Telegram/Resources/serein/regdate_points.LICENSE` 分发）的 212 个公开数据点之间线性插值，资料页显示“约某年某月”，晚于最后数据点时显示“晚于”；消息设置“波斯历（伊朗太阳历）”默认关闭，开启后上游的按日期格式化函数（聊天日期分隔、最后上线、媒体查看器等）改用 Qt `QCalendar` 的 Jalali 历换算，月份名称取自 Qt 的 CLDR 数据并随界面语言显示，日期选择器仍为公历） | Ni Na | Implemented | P3 |
| SG-MSG-13 | 双击自己的消息进行编辑（消息设置“交互”分区，默认关闭；开启后双击仍可编辑的自己发送的消息直接进入编辑，对话与话题、回复等列表都生效；其他消息仍执行上游聊天设置中的双击操作） | Na | Implemented | P3 |
| SG-MSG-14 | 资料页的联系人关系（隐私设置“在资料页显示联系人关系”，默认关闭：对方在你的联系人中时，资料页显示“互为联系人”或“仅在你的联系人中”，依据服务端下发的联系人与互为联系人标记，随标记变化即时更新；对方不在你的联系人中时服务端不提供该信息，不显示） | Ad | Implemented | P3 |

## COMPOSE 输入与发送

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-COMPOSE-01 | 输入框按钮显隐（D01–D11） | Ad Ni | Implemented | P1 |
| SG-COMPOSE-02 | 悬停不弹出面板、机器人命令先填入、占位文字（D12–D15） | Ni | Implemented | P2 |
| SG-COMPOSE-03 | 关闭自动 Markdown、默认不显示链接预览、发送与编辑时中西文加空格、默认代码语言、快捷回复（D16–D21） | Ni Na | Implemented | P1 |
| SG-COMPOSE-04 | 贴纸、GIF、语音、圆形视频、通话前确认（D22–D26） | Ni Na | Implemented | P1 |
| SG-COMPOSE-05 | 先转发后附言（D27） | Na | Implemented | P2 |
| SG-COMPOSE-06 | 草稿翻译与系统 AI 草稿（H04；输入框菜单“翻译草稿…”随写作设置“在输入框菜单中显示‘翻译草稿’”出现，该设置默认关闭） | Ni | Implemented | P2 |
| SG-COMPOSE-07 | 静音发送策略：从不、预设、始终（“按对话”由上游的静音发送开关提供；写作页新增“总是静音发送”，在 `Api::SendAction` 的统一挂钩中生效） | Ni | Implemented | P2 |
| SG-COMPOSE-08 | 文本替换规则（写作设置“文本替换”：每行一条“文本 => 替换内容”，最多 100 条，存为 proto 声明的配置文档；消息输入框在上游默认的即时替换之外加入这些规则，随设置变化实时生效，受上游“自动替换表情”开关控制） | Na | Implemented | P3 |
| SG-COMPOSE-09 | 格式工具栏（输入设置“选中文字时显示格式工具栏”，默认关闭；在消息输入框、说明文字等输入框中选中文字后，选区上方浮出粗体、斜体、下划线、删除线、等宽、剧透、引用、链接与清除格式按钮，已应用的格式高亮显示；按钮按输入框允许的格式显示，行为与上游快捷键和右键“格式”菜单一致） | Ni | Implemented | P3 |
| SG-COMPOSE-10 | 链接的内联机器人规则（写作设置“链接的内联机器人”：每行一条“@机器人 => 正则表达式”，最多 50 条，存为 proto 声明的配置文档；消息草稿只有一个不超过 256 字符、不以 @ 开头的链接且匹配规则时，上游的内联结果面板以该机器人和这个链接查询并显示结果，相当于在链接前输入 @机器人；这种由规则触发的查询不进入上游的内联模式，发送按钮、回车与 Esc 保持原样，直接发送仍发出链接本身；编辑消息时不生效；规则为空即关闭） | Na | Implemented | P2 |
| SG-COMPOSE-11 | 群主快速切换匿名发言（写作设置“在我创建的群组中快速切换匿名”，默认关闭：在自己创建的超级群组中，上游“以…身份发送”列表在服务端返回的身份之外加入群组本身与自己的账号，列表因此至少有两项，输入框左侧出现身份按钮，可在以群组身份匿名发言与以自己身份发言之间切换；选择、保存默认身份与发送沿用上游逻辑；只影响消息的身份列表，付费回应与直播的列表不变；聊天下次刷新列表时生效） | Na | Implemented | P3 |
| SG-COMPOSE-12 | 代码块语法高亮、输入框撤销与重做、表情与字体样式（由上游覆盖：代码块按 ```语言 由上游内置的 libprisma 高亮；输入框的撤销与重做由 Qt 标准右键菜单与快捷键提供；表情样式见 SG-MEDIA-09，主字体见 SG-APPEAR-06，双击消息的回复或回应由上游聊天设置提供，自己的消息另见 SG-MSG-13） | Na | Implemented | P3 |

## MENU 消息菜单

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-MENU-01 | 上游菜单项三态显隐：显示、隐藏、按住 Option／Alt 显示（E01–E14） | Ad Ni Na | Implemented | P1 |
| SG-MENU-02 | 复读、无引用复读、无引用转发，复读确认（消息菜单中位于“转发”之后：“复读”带来源转发到当前聊天或话题，“无引用复读”以隐藏发送者的方式由服务端转发到当前聊天，任何消息类型与整个相册原样复制，两者都要求当前聊天可以发言；“无引用转发”打开转发对话框并预设隐藏发送者，可转发到任意聊天；消息设置“复读前确认”默认关闭；三项菜单默认隐藏，在 SereinGram 设置的“消息菜单”页开启） | Ni Na | Implemented | P1 |
| SG-MENU-03 | 合并、反序、去署名后填入草稿，批量存入收藏夹；选择此人的消息（E18、E19） | Ni Na | Implemented | P2 |
| SG-MENU-04 | 媒体信息、消息截图、阅读转换切换（E20–E22） | Ad Ni | Implemented | P2 |
| SG-MENU-05 | 消息截图的简化引用样式（消息截图窗口新增“回复使用主题配色”，默认关闭；开启后只在生成截图时让回复与引用使用主题颜色，不改动界面设置“回复使用主题颜色”，也不重排聊天；截图配置升级为第 2 版，第 1 版配置读取时自动补上该项） | Ni | Implemented | P3 |
| SG-MENU-06 | 查看编辑历史、查看已删除内容（随 SG-HIST-01／02） | Ad Aa | Implemented | P1 |
| SG-MENU-07 | 读到此处、复制回调数据、消息详情等 AyuGram 菜单项及其显隐（“读到此处”已实现，可在菜单设置中隐藏；“按钮数据”“消息详情”已实现） | Ad | Implemented | P2 |
| SG-MENU-08 | 区间选择、批量取消置顶、快捷评价文本、文本菜单“提及”（“选择区间”“取消置顶所选消息”已实现，默认隐藏，可在菜单设置中显示；消息设置“最多选择 1000 条消息”默认关闭，开启后选择上限由 100 提高到 1000，转发与删除超过 100 条时按每批 100 条发送且不拆开相册；输入框中选中文字后右键“提及…”，输入用户名、t.me 链接或本账号已知的用户 ID，把选中文字变成对该用户的提及，写作设置“在文本菜单中显示‘提及…’”默认关闭；菜单设置“快捷评价”可填写两条文本，设置后消息菜单在“回复”下方出现“回复‘…’”，点击即以该文本回复这条消息，不会自动发送） | Na | Implemented | P2 |
| SG-MENU-09 | 消息菜单“设置提醒”（选择时间后把消息或整组相册保留来源转发到收藏夹的定时消息，到时以提醒通知；受保护的内容不显示该项；默认隐藏，可在菜单设置中显示） | Na | Implemented | P2 |

## MEDIA 媒体与贴纸

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-MEDIA-01 | 贴纸大小、隐藏时间、最近使用数量（最多 200）、隐藏群组与推荐贴纸、问候贴纸（F01–F08） | Ad Ni | Implemented | P1 |
| SG-MEDIA-02 | 关闭视频自动播放、GIF 播放控制（F09、F10） | Ad Ni | Implemented | P1 |
| SG-MEDIA-03 | 以文件发送的 MP4 保留视频预览（F11） | Ni | Implemented | P2 |
| SG-MEDIA-04 | 贴纸包列表导出与导入（F12、F13） | Na | Implemented | P2 |
| SG-MEDIA-05 | 查询贴纸包、表情包作者并打开资料（媒体设置“在贴纸包与表情包菜单中显示‘作者’”默认关闭；开启后贴纸包与表情包窗口右上角菜单出现“作者”：由贴纸包 ID 推算作者用户 ID，本地已知该用户时打开资料页；媒体设置“贴纸作者查询机器人”默认留空，填写内联机器人后，未知作者的 ID 会作为内联查询发给该机器人，从回答中取至多 3 个用户名逐个解析，只有解析出的用户正是该 ID 时才打开资料页，否则复制 ID；设置说明告知机器人运营者能看到查询的 ID） | Ad | Implemented | P2 |
| SG-MEDIA-06 | 下载时保留原始文件名（先核对上游现状）（由上游提供：`DocumentFileNameForSave` 默认使用原始文件名，核对于 2026-09-30） | Na | Implemented | P2 |
| SG-MEDIA-07 | 不经贴纸包收藏单个贴纸、收藏去重（由上游覆盖：消息中贴纸的右键菜单“添加到收藏”无需添加整个贴纸包，收藏列表由服务器按贴纸文档维护，不会重复） | Na | Implemented | P3 |
| SG-MEDIA-08 | 通话与录音降噪、语音增强（通话由上游覆盖：私人通话的 tgcalls 配置始终开启降噪，群组通话有上游的降噪开关；媒体设置“降低语音消息的背景噪音”默认关闭，开启后录制语音消息时在本设备上以群组通话所用的 RNNoise 逐 10 毫秒处理采样，钩子位于上游录音编码前的帧处理，从下一次录音开始生效） | Na | Implemented | P3 |
| SG-MEDIA-09 | 自定义表情资源包（由上游覆盖：设置中的“选择表情样式”可下载并切换多套表情图形资源，由 `chat_helpers/emoji_sets_manager` 管理；导入第三方资源包不在范围内） | Na | Implemented | P3 |
| SG-MEDIA-10 | 已保存音乐（由上游提供：资料页的音乐收藏由 `Data::SavedMusic` 管理并与服务端同步；为缺少封面的音频向外部服务查询封面会把曲目信息发给第三方，列入明确排除） | Ad Ni | Implemented | P3 |
| SG-MEDIA-11 | 在桌面发布动态（媒体页“在本设备发布动态”默认关闭；打开后主菜单出现“发布动态”，可发布动态的频道在对话菜单出现“发布动态”：先以 `stories.canSendStory` 确认额度，再选择一张图片或一段视频（也可以把图片或视频粘贴、拖放到说明输入框中替换），超过 60 秒的视频先在上游视频编辑器中截取不超过 60 秒的片段，经上游的转码流程上传；图片按移动端的做法铺到 1080×1920 画布上，不是 9:16 的部分用模糊放大的原图填充，视频原样上传；说明支持格式，长度上限取服务端配置；可见范围可选所有人、联系人（都可排除部分人）、密友或指定的人，密友名单从服务端读取并可直接编辑；Premium 账号可选 6／12／24／48 小时；可选择消失后保留在个人页、是否允许截图与转发；框内显示准备、上传与发布进度，失败时按服务端错误说明原因，例如有效动态数量、每周与每月额度、频道助力不足；已发布的动态可在媒体查看器菜单“编辑动态”中修改说明与可见范围，可见范围按服务端保存的规则预填，含本应用不能表示的规则（如按群组成员）时只修改说明；他人可转发的公开动态可在同一菜单“转发到我的动态”，沿用原媒体并带上来源，可另写说明并选择可见范围与有效期；实现位于 `features/stories`，上游只在媒体查看器菜单加了一行挂钩） | Na Aa D | Implemented | P2 |

## TRANS 翻译、转写与 AI

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-TRANS-01 | 服务实例：OpenAI 兼容与 DeepL 协议、连接测试、密钥存入系统凭据库（H03） | Ni | Implemented | P1 |
| SG-TRANS-02 | 翻译服务选择与草稿翻译（H01） | Ad Ni | Implemented | P1 |
| SG-TRANS-03 | 更多服务：Google、Yandex、Microsoft、Transmart、DeepLX、Anthropic 与 Gemini 协议（已接入 Google、Yandex、Transmart、DeepLX、Anthropic 原生协议，Gemini 走官方 OpenAI 兼容端点；协议构造、解析与请求头集中在 `services/translation_protocol` 与 `services/model`。Microsoft 免费接口 `edge.microsoft.com/translate/auth` 已下线（404），改接 Azure Translator v3 官方接口：订阅密钥存系统凭据库，区域字段可选，服务配置升级到 v2 并自动迁移 v1） | Ad Ni Na | Implemented | P1 |
| SG-TRANS-04 | 简繁转换改用 OpenCC 词组级转换，三平台可用（替代现有系统逐字转换）（按 ADR-0005 实施：OpenCC ver.1.4.2 子模块编译为静态库，四个文本词典随资源分发并在首次使用时解压到 `tdata/serein/opencc-1.4.2`；Linux 也可用；实体与代码片段保持原样） | Na | Implemented | P1 |
| SG-TRANS-05 | 语音转写服务（H02） | Ni | Implemented | P2 |
| SG-TRANS-06 | 系统 AI 草稿预览（H04，macOS） | Ni | Implemented | P2 |
| SG-TRANS-07 | Instant View 与选中文本翻译（聊天中选中文本的“翻译”与消息翻译统一使用所选翻译服务；服务设置“翻译即时预览页面”默认关闭，开启后即时预览菜单出现“翻译页面”与“显示原文”：页面按富文本消息的长度、块数与媒体上限分段，经 Telegram 的富文本翻译接口逐段译成翻译语言，保留版式、图片与链接；无法序列化或超出上限的块保留原文，中途失败时显示已译部分并提示，页面内容更新后回到原文） | Na | Implemented | P2 |
| SG-TRANS-08 | 按对话自动翻译，话题、对话、账号、全局四级继承（服务设置“非 Premium 用户也可翻译整段对话”默认关闭；开启且所选翻译服务为外部服务或系统翻译时，上游的整段对话翻译（翻译栏、对话菜单“翻译”）对非 Premium 用户可用，跟踪器改用所选服务，外部服务把多条消息的待译片段合并后按服务的单次上限分批请求；Telegram 自己的翻译仍需 Premium；继承关系：全局由上游的翻译按钮与“不翻译的语言”提供；账号级由服务设置“自动翻译对话”提供，按账号保存、默认关闭，开启且本账号可翻译整段对话时，出现翻译提示的对话立即翻译成目标语言，与频道的自动翻译走同一路径；对话级沿用上游按对话的翻译状态，关闭过翻译的对话不会自动翻译；话题共用所在对话的翻译状态，不单独设置） | Ni | Implemented | P3 |
| SG-TRANS-09 | LLM 上下文翻译与摘要（摘要已实现：所选翻译服务是带模型的 OpenAI 兼容或 Anthropic 服务时，对话菜单出现“总结最近的消息”，把最多 60 条最近加载的文字消息以 JSON 数组发送给该服务，按对话的译入语言返回要点，窗口中注明发送对象并可复制；上下文翻译已实现：服务设置“附带前文作为上下文”默认关闭，开启后用这类服务翻译聊天消息时，把该消息之前最多 6 条已加载的文字消息及作者作为仅供参考、不翻译的上下文写入提示词） | Ni | Implemented | P3 |

## NET 网络与代理

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-NET-01 | 代理备注、测速、排序、清理失效节点、导入导出（上游代理管理已提供从剪贴板批量导入、全部删除、分享整个列表、延迟显示与自动切换；服务设置新增“按延迟排序代理”与“移除不可用的代理”，用上游公开的 `MTP::StartProxyCheck` 逐个检测（最长 15 秒），排序时可用代理按延迟在前、未检测的 Web 代理其次、不可用的最后，清理时保留正在使用的代理，检测期间列表若被改动则放弃；服务设置“代理备注”列出当前代理逐个填写备注（单行，至多 64 个字符，至多 200 条），备注按地址与端口保存并显示在上游代理列表的地址之后，钩子为代理行标题拼接处的一行） | Na | Implemented | P2 |
| SG-NET-02 | 代理自动切换与 VPN 感知（上游代理设置已提供按超时自动切换；服务设置“VPN 开启时暂停代理”默认关闭，开启后每 15 秒用不发包的 UDP 连接探测发往 Telegram 数据中心的流量经过哪个网络接口，接口类型或名称表明是 VPN 隧道时关闭代理并提示，VPN 断开后恢复原代理；暂停状态持久保存，重启后仍能恢复；PPPoE 与蜂窝网卡不视为 VPN） | Na | Implemented | P3 |
| SG-NET-03 | 代理订阅（SIP008、Clash 等），新协议只通过外部代理程序接入（服务设置“代理订阅”：填写 HTTPS 地址，“立即更新”下载后提取其中的 tg://proxy、t.me/proxy 与 t.me/socks 链接（最多 200 个，响应不超过 1 MB），经上游链接解析后只加入代理列表中尚未存在的条目并提示数量；SIP008、Clash 等其他协议需要外部代理程序，不在客户端内解析） | Na | Implemented | P3 |
| SG-NET-04 | 自定义 DoH 与 IP 策略（服务设置“自定义 DNS-over-HTTPS 服务器”默认关闭，填写在 `/dns-query` 提供 JSON 接口的主机名后，上游查询备用配置与解析代理域名时先于 Google 与 Cloudflare 请求该服务器；钩子以模板插入上游两处请求列表，不引用上游类型；IP 策略由上游连接设置中的“尝试通过 IPv6 连接”提供） | Na | Implemented | P3 |
| SG-NET-05 | 上传与下载加速（服务设置“加快上传与下载”，默认关闭。上游按请求耗时自适应：下载把每个数据中心的连接从 1 条逐步增加到 8 条、每条连接的在途窗口从 4 个分片增加到 16 个，上传在请求都较快时逐步增加连接、每条连接最多 1 MB 在途；延迟高的网络（例如经远程代理）中请求很难被判为快速，连接数与窗口长期停在起点。开启后下载一开始就使用 4 条连接与最大窗口，上传每条连接最多 4 MB 在途，上限与按耗时增减连接的规则不变；服务器仍按账号限速） | Na | Implemented | P2 |
| SG-NET-06 | 以 Android 客户端身份打开小程序（服务设置“以 Android 客户端身份打开小程序”，默认关闭；开启后申请小程序、主应用、附件菜单与入群验证小程序时向服务器报告 Android 平台，小程序据此提供移动端的功能与布局，JS 桥保持不变） | Aa | Implemented | P3 |
| SG-NET-07 | 数据中心状态（服务设置“数据中心状态”：并发测量 DC1–DC5 的响应时间，用上游的 MTProto 连接组件逐个连接并读取握手往返时间，10 秒内无响应记为不可用；选用 SOCKS5、HTTP 或 MTProto 代理时经该代理测量，未用代理或代理为 Web 类型时直连测量，窗口顶部注明测量方式；标出当前账号所在的数据中心，可重新检测；可用、不可用与检测中的文字沿用上游代理列表的翻译） | Na | Implemented | P3 |
| SG-NET-08 | 使用系统 DNS 解析代理域名（服务设置“使用系统 DNS”，默认关闭；开启后以域名填写的代理服务器改由操作系统的解析器（`QHostInfo`）查询，IPv4 地址在前，结果交给上游原有的逐个 IP 尝试流程，五分钟后重新解析；不再经 Google 与 Cloudflare 的 DNS-over-HTTPS 查询，自定义 DoH 服务器也不用于代理，备用配置的查询不受影响；用于这些服务被屏蔽、以域名填写的代理一直超时的网络；上游只在 `DomainResolver::resolve` 加了一个条件） | Sw | Implemented | P2 |

## ACCT 账号

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-ACCT-01 | 放开本地登录账号数量上限（界面设置“允许登录最多 20 个账号”，默认关闭；存储校验上限随之放宽，Premium 宣传中的官方上限不变；账号列表的“添加账号”按钮按 `maxAccounts()` 与官方上限中较大者显示，开启后可加到 20 个） | Na | Implemented | P2 |
| SG-ACCT-02 | 账号作用域设置的双账号隔离验收（自动化部分已覆盖：两个账号的存储互不可见、设备存储拒绝账号选项、设置导出不含账号值、对话列表与过滤、幽灵、历史等按账号保存的选项逐项断言作用域，`Options::Get` 在作用域不符时断言失败；两个账号同时登录的现场验收通过后记为 Verified） | D | Implemented | P1 |

## ADMIN 群组与资料管理

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-ADMIN-01 | 群组资料页管理快捷入口集合（聊天设置“资料菜单中的群组管理”，默认关闭：在可管理的群组资料菜单中加入成员、管理员、已移除的用户与最近操作，显示条件与上游“管理群组”一致，直接打开上游对应的列表与日志；权限编辑需要复用上游保存逻辑，仍从“管理群组”进入） | Ni | Implemented | P2 |
| SG-ADMIN-02 | 删除群内自己的全部消息、全部解除屏蔽、无成员建群、升级为超级群（“删除我的全部消息”与“升级为超级群组”随聊天设置“管理入口”出现，该设置默认关闭；群组与超级群的对话菜单“删除我的全部消息”：确认后按消息 ID 向前分页搜索自己发出的消息，每页 100 条，交给上游 `Histories::deleteMessages` 为所有人删除，完成后提示删除数量；隐私设置“解除屏蔽所有用户”：确认后按页取回屏蔽列表，逐个经上游 `Api::BlockedPeers` 解除，重复出现或失败即停止并提示已解除数量；无成员建群由上游覆盖：新建群组选成员时不选任何人也能创建；自己创建的基础群资料菜单“升级为超级群”：确认后调用上游 `ApiWrap::migrateChat`，成功后打开新的超级群） | Na | Implemented | P3 |
| SG-ADMIN-03 | 删除对话框的默认勾选项（仍逐次确认）（消息设置“删除”分组，默认全部关闭：删除或清空私聊时默认勾选“同时为对方删除”，群组中同一勾选项代表为所有成员删除，因此不受影响；管理消息时默认勾选“举报垃圾信息”“删除此用户的全部消息”“封禁用户”，经上游 `DefaultModerateMessagesBoxOptions` 覆盖所有删除入口，按住 Ctrl 仍全选；上游删除消息时的“为所有人删除”已有记住选择） | Ni | Implemented | P3 |
| SG-ADMIN-04 | 频道本地别名、彩色管理员头衔（频道本地别名由 SG-PRIV-04 覆盖：本地名称适用于用户、群组与频道；彩色管理员头衔由上游覆盖：消息头部的头衔按群主、管理员与普通成员分别使用 `rankOwnerFg`、`rankAdminFg` 与 `rankUserFg`，群主与管理员的头衔另有同色的半透明底） | Ni | Implemented | P3 |

## TG Telegram 功能完整性

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-TG-01 | Telegram 全部功能保留；Serein 功能全部关闭时行为与上游一致（核心测试 `TestNeutralDefaults` 把 13 个设置页的全部选项注册到一起，逐项要求默认值为关闭、零或空；只有 8 项按写明的理由列入例外：两个空的快捷回复槽位、幽灵模式总开关下的 4 个子项、替代被关闭的官方更新器的 GitHub 更新检查、等于上游尺寸的 100% 贴纸缩放、依赖默认关闭的历史记录的已删除消息淡化；新增非中性默认值或例外失效都会让测试失败；同一测试要求上游的消息菜单项默认显示、Serein 新增的菜单项默认隐藏，只有出现前提本身默认关闭的 6 项例外：编辑历史、已删除消息、不记录此对话依赖默认关闭的消息记录，“标记已读到此处”依赖默认关闭的幽灵模式，两条快捷评价默认未设置；2026-10-01 逐项审计了 Serein 向上游菜单添加入口的全部位置（消息菜单、对话菜单、资料菜单、主菜单、托盘菜单、贴纸包菜单、文件夹菜单与输入框菜单），原先常驻的“跳到开头”“删除我的全部消息”“设置本地名称”“升级为超级群组”、主菜单与托盘的快捷入口、文件夹的“仅显示我管理的群组和频道”、“翻译草稿…”与贴纸包“作者”均改为由默认关闭的选项控制；其余钩子多为读取选项的生成门面，抽查的手写钩子（链接改写、文本预处理、账号上限、排序等）在默认值下走上游路径） | T | Implemented | P0 |
| SG-TG-02 | 每个上游版本（beta 与 stable）同步一次，同步后三平台 CI 通过才发布（同步工具 `tools/serein/upstream_sync.py` 已就绪，`serein-upstream.yml` 每周一自动合并上游 `dev`、开同步 PR 并触发守卫与三平台构建，流程见 ADR-0001） | T | In Progress | P0 |
| SG-TG-03 | 上游侵入预算：上游文件数、新增行数、挂钩数在 CI 中统计并设上限 | D | Implemented | P0 |

## 需求族映射

手机版 Nagram 的需求族与本表各族的对应关系，用于逐项核对覆盖率：

| Nagram 需求族 | 本表 |
| --- | --- |
| 品牌、主题与外观 | APPEAR、BRAND |
| 聊天列表与导航 | CHATS |
| 消息显示 | MSG |
| 输入与文本 | COMPOSE |
| 消息菜单与批量操作 | MENU |
| 媒体与贴纸 | MEDIA |
| 翻译与 LLM | TRANS |
| 语音转写 | TRANS |
| 过滤与规则 | FILTER |
| 隐私与本地锁 | PRIV |
| 回执与在线状态 | GHOST |
| 本地历史 | HIST |
| 消息截图 | MENU |
| 资料与群管理 | MSG、ADMIN |
| 网络与代理 | NET |
| 链接与外部集成 | FILTER、TRANS |
| 配置、同步与更新 | CORE、PLAT |
| 通知呈现 | APPEAR、PRIV |

## 明确排除

- 手机专属交互：振动、滑动手势、底栏、前后摄像头、距离传感器、移动推送。
- 桌面不支持的秘密聊天相关项（截图、屏蔽发起秘密聊天）。
- 冒充官方客户端、使用官方客户端 API 凭据（AyuGram 的做法）。
- 内置 VMess、Shadowsocks、SSR、Trojan 代理核心；OpenKeychain 集成。
- 运行时切换测试服务器（仅作为开发构建配置）。
- 从外部服务为音频补全封面：会把收听的曲目信息发给第三方。
- 本地隐藏已登录账号：需要同时改动上游的账号列表、托盘菜单、切换快捷键、通知与未读计数，侵入面过大。
- 本地 Premium 外观（原 SG-PRIV-07）：只改变本机显示、不带来任何权益，容易与真实状态混淆；有实际用处的客户端能力（如非 Premium 用户翻译整段对话）已单独实现。
- 自定义等宽字体与 Material 风格开关动画（原 SG-APPEAR-11）：两者都由 lib_ui 绘制且没有公开接口，只能修改子模块。
- 跨设备同步已读状态与消息历史（原 SG-SYNC-01）：需要自建服务端与账号体系，超出客户端的范围。
- 按账号设置本地密码：上游的本地密码作用于整个应用，按账号锁定需要改动账号切换、通知、托盘与未读计数等多处上游代码，侵入面过大。
- 内置 WebSocket 代理：需要在客户端内实现经 Telegram 网页端点的 WebSocket 传输与本地 SOCKS5 转发，静态构建的 Qt 不含 WebSockets 模块；外部的 tg-ws-proxy 等工具配合上游的 SOCKS5 代理设置可达到同样效果。
- 会话导出与导入：导出的授权密钥等同于账号凭据，一旦泄露即可被直接登录。
- Liquid Glass 色调、推送服务选择：分别是 iOS 与 Android 的系统特性，桌面端没有对应能力。
