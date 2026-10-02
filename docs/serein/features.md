# 功能矩阵（TODO 清单）

本表是 SereinGram 功能状态的唯一来源，状态与优先级定义见 [产品规格](README.md#3-标识与状态)。

来源缩写：`Ni` Nagram iOS · `Na` Nagram Android（含 NekoX／Nekogram 继承项）· `Ad` AyuGram Desktop · `Aa` AyuGram Android · `T` Telegram Desktop 已有 · `D` 桌面补充。

部分功能继承自 Nagram-qt，代码与核心测试已完成但多数缺少现场验收，统一记为 `Implemented`。

核对口径：

- AyuGram：`docs.ayugram.one/desktop` 功能列表、AyuGramDesktop 与 AyuGram4A 的 README（2026-09-30 抓取）。
- 手机版 Nagram：Nagram iOS 的集中偏好，Nagram Android 继承自 NekoX／Nekogram 的偏好、菜单动作与无开关能力，以及 Nagram Android README（2026-09-30 整理）；按下文“需求族映射”归入本表各族，排除项见末节。

## PLAT 平台与交付

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-PLAT-01 | macOS 通用二进制（arm64 + x86_64）构建与 DMG（依赖与应用默认按 `x86_64;arm64` 构建；CI 用 `lipo -verify_arch` 校验两种架构后打包 DMG 上传，2026-10-01 首次通过，`test_serein` 同时运行；签名与公证需要 Apple Developer ID 证书，尚未接入） | T | In Progress | P0 |
| SG-PLAT-02 | Windows x64 构建、安装包与便携版（便携版为单个 `SereinGram.exe`；CI 每次构建都以 Inno Setup 编译 `setup.iss` 生成安装包，失败即构建失败，发布时命名为 `SereinGram-windows-x64-setup.exe`；签名待做） | T | In Progress | P0 |
| SG-PLAT-03 | Linux x86_64 静态构建（Rocky Linux 8 容器）与 tar 包（CI 打包 `SereinGram.tar.gz` 与 `SereinGram-x86_64.AppImage` 上传，AppImage 用固定版本与校验和的 appimagetool 1.9.1 和 type2 运行时生成；ffcfe2a 起 Linux 工作流全绿：应用与 `test_serein` 构建通过、全部核心测试通过、产物上传成功，构建失败时也保存 ccache，热缓存下整轮约 17 分钟） | T | Implemented | P0 |
| SG-PLAT-04 | 三平台 CI：构建、`test_serein`、全部守卫（`serein-{mac,win,linux}.yml` 构建应用与 `test_serein`，`serein-guards.yml` 在每次推送运行全部守卫、核心测试与 commitlint 且持续通过；三平台的应用构建与 `test_serein` 均已通过，Windows 同时生成安装包，macOS 同时生成通用 DMG，Linux 同时生成 AppImage、`.deb` 与 `.rpm`） | D | Implemented | P0 |
| SG-PLAT-05 | 发行版打包：Flatpak 清单、AUR PKGBUILD、`DESKTOP_APP_USE_PACKAGED` 依赖清单（Arch Linux：`packaging/arch/PKGBUILD` 以系统库构建 `sereingram-desktop-git`，依赖清单与 Arch 官方 telegram-desktop 一致，tde2e 按 `snap/snapcraft.yaml` 锁定的 tdlib 提交现场编译，守卫 `check_packaging.py` 要求 PKGBUILD 与 Flatpak 清单锁定的 tdlib、tg_owt、tlottie、patches 提交与 Qt 版本都与 snap 配方一致；打包者通过 `SEREIN_API_ID` 与 `SEREIN_API_HASH` 提供自己的凭据，缺少时构建直接报错，绝不使用官方 Telegram 凭据；CI 工作流 `serein-arch.yml` 在 Arch 容器中用 makepkg 构建当前提交、安装后检查文件与动态库并上传包，已在 CI 中构建并安装通过；以系统库构建时不启动 GitHub 更新检查，设置页改为说明由系统包管理器更新；Flatpak：`packaging/flatpak/io.github.eltavine.SereinGram.yml` 基于 GNOME 51 运行时，依赖模块与 Flathub 的 Telegram 清单一致，构建本地检出，凭据来自被忽略的 `api_credentials.local.cmake`；CI 工作流 `serein-flatpak.yml` 构建并上传 `.flatpak` 包，已在 CI 中生成；尚未提交到 Flathub；Debian／Ubuntu 与 Fedora／openSUSE：Linux 工作流用 nFPM（`packaging/nfpm/`）把 CentOS 基线构建的同一个程序连同桌面入口、图标与 AppStream 元数据打成 `.deb` 与 `.rpm`，与 AppImage 一起上传，覆盖 glibc 2.28 及以上的主流发行版） | Ad | Implemented | P2 |
| SG-PLAT-06 | Windows arm64 构建（暂从 CI 矩阵移除：上游 ffmpeg n8.1.3 在 arm64 上生成的 `epel_neon.d` 依赖文件格式错误，导致依赖构建失败） | T | Planned | P2 |
| SG-PLAT-07 | 独立更新检查（GitHub Releases，默认不自动下载）（界面设置“在 GitHub 上检查更新”默认开启：启动 30 秒后及每 24 小时查询最新正式版，只接受 github.com 的发布页链接，发现新版本时提示链接，不自动下载；官方更新通道在构建中关闭；推送 `v*` 标签时三平台工作流把产物上传到草稿预发布 Release，核对后手动发布） | D | In Progress | P2 |
| SG-PLAT-08 | API 凭据构建期注入（Secrets 或本地文件），禁止使用官方客户端凭据 | D | Implemented | P0 |

## BRAND 品牌与身份

Nagram 的品牌政策要求分支使用不同品牌并替换 Nagram 名称与图标，以下各项是发布前提。

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-BRAND-01 | 应用名 SereinGram：可执行文件、关于页、托盘、通知、安装器文案 | D | Implemented | P0 |
| SG-BRAND-02 | 自有图标资产，替换全部 Nagram 图标（当前为生成的临时占位图标，正式图标待定） | D | Implemented | P0 |
| SG-BRAND-03 | 应用 ID、数据目录、便携目录、Windows AppUserModelID、安装器 ID 与通知激活器 GUID | D | Implemented | P0 |
| SG-BRAND-04 | 源码、发布与问题反馈链接指向 `eltavine/SereinGram` | D | Implemented | P0 |
| SG-BRAND-05 | 代码更名：`nagram` → `serein`（目录、命名空间、文案键、存储键、CMake、测试目标、工作流） | D | Implemented | P0 |

## CORE 基础设施

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-CORE-01 | proto3 schema 作为设置、结构化配置、导出包与历史记录的唯一声明来源；Buf lint 与 breaking 检查（13 个设置 proto 生成 191 个选项，另有配置、导出格式与历史记录 proto；菜单配置、重复发送确认、消息过滤、链接规则、消息截图、翻译与转写服务、“优先系统 AI”均由设置 proto 以自定义界面字段声明，`buf breaking` 可发现键名改动；仍在 C++ 中手写的只剩本地别名 1 个选项：它的存储校验要把每个键解析成应用层的对话 ID 并检查类型，核心测试无法链接，改用纯函数校验会让含非法键的导入被接受、再在使用时整体失效） | D | Implemented | P0 |
| SG-CORE-02 | 代码生成器：注册表元数据、C++ 值类型、JSON 编解码、校验（均已生成；既有结构化配置已迁移 10 个：链接规则、快捷回复、消息过滤、主菜单、消息菜单、翻译与转写服务、历史排除列表、本地别名、贴纸目录导出文件、消息截图设置；所有存储与导出格式都由 proto 声明；过滤、链接规则、主菜单与翻译转写服务的设置界面均已改用生成的结构体读写，JSON 只保留在编解码与校验层；支持字符串键的 map 字段、double 与可空字段） | D | Implemented | P0 |
| SG-CORE-03 | 存储端口与适配器：设备偏好、账号偏好、历史库（选项存储分设备与账号两个作用域；历史库经 `Ports::HistoryStore` 端口由 Qt SQL 适配器实现，记录整体用基于本地密钥派生的 AES-GCM 加密，缓存媒体副本用同一密钥加密） | D | Implemented | P0 |
| SG-CORE-04 | 上游挂钩门面 `serein/hooks`，上游文件只调用门面（设置选项的门面由 proto 生成到 `serein/hooks/gen`，面向上游的薄接口头文件已移入 `serein/hooks/<领域>/`，并可脱离应用代码单独通过语法检查；直接包含内部头文件的上游文件已从 87 降到 0，并由预算守卫锁定；依赖上游嵌套类型的门面声明为函数模板，在实现文件中对该类型显式实例化，门面本身不包含应用代码） | D | Implemented | P0 |
| SG-CORE-05 | 功能模块注册与生命周期（应用、会话、窗口作用域）：应用与会话作用域已由 `serein/app/modules.cpp` 模块表统一分发（对话排序已从上游窗口构造函数移到会话作用域）；窗口作用域由 `Serein::Hooks::OnWindowStarted` 分发，首个使用者是最近会话记录；主菜单条目统一由 `Serein::Hooks::FillMainMenu` 添加 | D | Implemented | P0 |
| SG-CORE-06 | 设置页与搜索索引由 schema 元数据生成（开关、选项、数值、文本、子页）：开关行、小标题、说明、依赖开关与自定义行位置已由 proto 生成 `AddLayout`，界面、聊天、消息、写作、菜单、媒体、隐私（含幽灵与历史）七页已迁移；数值输入行（范围取自 gte/lte 规则，0 的文字与复数格式由 `number` 选项声明）已生成；单选行（按序语言键或 `in` 规则加后缀）已生成；文本行（`text` 选项声明占位语言键，未设置时行标签显示占位文字，`disabled_by` 使该行在对应开关打开时隐藏）已生成，“已编辑/已删除标记文字”已改用生成行；子页由页面选项 `subpage` 声明标题、图标与搜索关键词，生成父页上的入口按钮 `AddSubpageButton` 以及分区标题、图标常量，分区类本身沿用各页相同的手写样板；幽灵模式与“已删除与已编辑消息”已拆成隐私页下的子页 | D | Implemented | P0 |
| SG-CORE-07 | 英文、简体、繁体内置文案与一致性检查 | Ni Na | Implemented | P0 |
| SG-CORE-08 | 配置管理：已修改项、导出、导入差异预览、诊断信息（J01–J04）；“恢复默认设置”先列出本设备上所有已修改的可导出设置及其当前值，确认后一次恢复为默认值，按账号保存的设置不受影响 | Ni Na | Implemented | P1 |
| SG-CORE-09 | 守卫：源文件 ≤ 1000 行、模块依赖方向、上游侵入预算、生成代码漂移 | D | Implemented | P0 |
| SG-CORE-10 | 测试：纯逻辑单元测试与 `-testagent` 界面场景 | D | Implemented | P0 |
| SG-CORE-11 | 更多界面语言的社区翻译平台接入（Serein 字符串按界面语言加载任意 `langs/serein/<语言代码>.strings`，缺失的键回退英文，CMake 扫描目录自动生成资源清单；核心测试要求中文译文完整、其他译文的键与占位符与英文一致；`crowdin.yml` 与同步工作流 `serein-crowdin.yml` 已就绪：在 GitHub Secrets 中配置 Crowdin 项目 ID 与访问令牌后，英文源文件改动时上传，每周下载译文并向 develop 提交 PR） | Ad Na | In Progress | P3 |

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

SG-HIST-05 的“已编辑”标记文字已由 C09 实现，删除标记随 SG-HIST-01 落地。

当前进度：服务器删除与编辑前的快照经 `serein/hooks/history.h` 写入加密的 SQLite 历史库（记录器、Qt SQL 存储、AES-256-GCM 加密均有测试）；设置页可开启保存。原位显示、按对话浏览与菜单查看版本待做。

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
| SG-PRIV-07 | 本地 Premium 外观，仅本地显示，不伪造服务端权益 | Ad Aa | Planned | P3 |
| SG-PRIV-08 | 受保护内容的本地复制与保存（复制文字、保存媒体与截图已放开，转发仍由服务端拒绝；受保护的动态可截图，并与普通动态一样可由 Premium 用户保存，不绕过 Premium 限制） | Ad Na | Implemented | P2 |
| SG-PRIV-09 | 设置锁与本地账号隐藏（设置锁已实现：隐私设置“用本地密码锁定 SereinGram 设置”默认关闭，设置了上游本地密码时，打开任一 SereinGram 设置页（包括从设置搜索直达的子页）先显示密码输入，用上游 `checkPasscode` 校验，每次启动及应用被锁定后重新上锁；本地账号隐藏需要改动上游账号列表，待做） | Ni | In Progress | P3 |
| SG-PRIV-10 | 检测到录屏软件时自动开启主播模式（隐私设置“运行直播或录屏软件时自动开启”，默认关闭：开启后每 5 秒在后台线程列出进程——Windows 用 Toolhelp 快照、macOS 用 libproc、其他平台读 `/proc/*/comm`——发现 OBS、Streamlabs、XSplit、vMix、Bandicam 等常见软件即打开主播模式，软件退出后恢复原状，期间手动改过则不再动它） | Ad | Implemented | P3 |
| SG-PRIV-11 | 设备详情中的登录时间、API ID 与官方应用标记（隐私设置“显示更多设备信息”，默认关闭：开启后“设置 > 设备”中每个会话的详情在系统版本之后加入服务端返回的登录时间、API ID 以及是否为官方应用，便于识别第三方客户端登录的会话；数据来自上游已请求的会话列表，不额外发送请求） | Ad | Implemented | P3 |

## APPEAR 界面与外观

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-APPEAR-01 | 气泡与头像圆角（A02–A04） | Ad Ni | Implemented | P1 |
| SG-APPEAR-02 | 消息宽度、频道宽消息、隐藏气泡尾巴、引用配色、回复缩略图、忽略对话主题（A05–A10） | Ad Ni | Implemented | P1 |
| SG-APPEAR-03 | 主菜单标题、顺序、显隐与节日装饰（A11） | Ni | Implemented | P1 |
| SG-APPEAR-04 | 应用图标角标、通知延迟（A12–A14） | Ni | Implemented | P2 |
| SG-APPEAR-05 | 界面半角标点（A15） | Ni | Implemented | P2 |
| SG-APPEAR-06 | 主字体与等宽字体自定义（主字体由上游 `customFontFamily` 提供；等宽字体的字体族写死在 lib_ui，需上游支持后接入） | Ad Ni | In Progress | P2 |
| SG-APPEAR-07 | 应用图标选择（界面设置“应用图标”：选择至少 64×64 的 PNG、JPEG 或 WebP 图片，缩到 512 像素以内存为 `tdata/serein_app_icon.png`，启动时经上游 `Window::OverrideApplicationIcon` 替换窗口与任务栏图标，macOS 另经 `base::SetCustomAppIcon` 替换程序坞与访达图标；可恢复默认；内置图标方案待有正式图标素材后再加） | Ad Ni | Implemented | P2 |
| SG-APPEAR-08 | 以频道身份发言时显示频道徽标（消息页开关，默认关闭：超级群中以广播频道身份发送的消息在名字右侧显示上游已有的“频道”标记；匿名管理员以群身份发言时不显示） | Ad Ni | Implemented | P2 |
| SG-APPEAR-09 | 连续的文字、贴纸、圆形视频消息组的气泡尾巴修正（由上游覆盖：上游区分逻辑上的连续与气泡上的连续（`BubbleAttachedToNext`／`BubbleAttachedToPrevious`），下一条是贴纸、圆形视频等无气泡消息时文字气泡保留尾巴与大圆角，反之亦然；2026-10-01 对照 AyuGramDesktop 当前源码，相关的 `setAttachToNext` 与 `countMessageRounding` 与上游一致） | Ad | Implemented | P3 |
| SG-APPEAR-10 | 圆角贴纸（媒体设置“贴纸圆角”默认关闭；开启后聊天中的静态贴纸以大圆角生成图像并随图像缓存，动画与视频贴纸逐帧加圆角，大号表情与自定义表情不处理；钩子位于上游贴纸的图像生成与逐帧绘制） | Ad | Implemented | P3 |
| SG-APPEAR-11 | Material 风格开关动画 | Ad | Planned | P3 |
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
| SG-MSG-08 | 消息详情：日期、转发来源、贴纸包与表情包作者（消息菜单“消息详情”：消息与对话、发送者 ID（Bot API 格式）、带秒的发送与编辑时间、转发来源与原始时间、浏览数、贴纸包名称与链接、贴纸包作者） | Ad Na | Implemented | P2 |
| SG-MSG-09 | 内联按钮回调数据查看与复制（消息菜单“按钮数据”：列出内联按钮的文字与数据，点击复制；不可打印的数据以 base64 显示；默认隐藏，可在菜单设置中开启） | Ad | Implemented | P2 |
| SG-MSG-10 | 语音与圆形视频拖动进度（先核对上游现状）（由上游提供：语音消息 `VoiceSeekClickHandler` 与圆形视频 `VideoMessageSeek` 均支持拖动进度，核对于 2026-09-30） | Ad | Implemented | P2 |
| SG-MSG-11 | 反应时间显示秒（反应与已读列表的时间随“消息时间显示秒”选项显示到秒） | Ad | Implemented | P3 |
| SG-MSG-12 | 注册日期估算（标注估算来源）、波斯日历（隐私页“显示估算的注册时间”，默认关闭：按用户 ID 在 WizardLoop/CreationDate（MIT，提交 f37728802d36，许可证随 `Telegram/Resources/serein/regdate_points.LICENSE` 分发）的 212 个公开数据点之间线性插值，资料页显示“约某年某月”，晚于最后数据点时显示“晚于”；消息设置“波斯历（伊朗太阳历）”默认关闭，开启后上游的按日期格式化函数（聊天日期分隔、最后上线、媒体查看器等）改用 Qt `QCalendar` 的 Jalali 历换算，月份名称取自 Qt 的 CLDR 数据并随界面语言显示，日期选择器仍为公历） | Ni Na | Implemented | P3 |
| SG-MSG-13 | 双击自己的消息进行编辑（消息设置“交互”分区，默认关闭；开启后双击仍可编辑的自己发送的消息直接进入编辑，对话与话题、回复等列表都生效；其他消息仍执行上游聊天设置中的双击操作） | Na | Implemented | P3 |

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

## MENU 消息菜单

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-MENU-01 | 上游菜单项三态显隐：显示、隐藏、按住 Option／Alt 显示（E01–E14） | Ad Ni Na | Implemented | P1 |
| SG-MENU-02 | 复读、无引用复读、无引用转发，复读确认（E15–E17、E24） | Ni Na | Implemented | P1 |
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
| SG-MEDIA-10 | 音乐封面服务与已保存音乐（已保存音乐由上游提供：资料页的音乐收藏由 `Data::SavedMusic` 管理并与服务端同步；为缺少封面的音频从外部服务取封面需要改动上游的播放器与音频缩略图绘制，并会把曲目信息发给第三方，待做） | Ad Ni | In Progress | P3 |

## TRANS 翻译、转写与 AI

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-TRANS-01 | 服务实例：OpenAI 兼容与 DeepL 协议、连接测试、密钥存入系统凭据库（H03） | Ni | Implemented | P1 |
| SG-TRANS-02 | 翻译服务选择与草稿翻译（H01） | Ad Ni | Implemented | P1 |
| SG-TRANS-03 | 更多服务：Google、Yandex、Microsoft、Transmart、DeepLX、Anthropic 与 Gemini 协议（已接入 Google、Yandex、Transmart、DeepLX、Anthropic 原生协议，Gemini 走官方 OpenAI 兼容端点；协议构造、解析与请求头集中在 `services/translation_protocol` 与 `services/model`。Microsoft 免费接口 `edge.microsoft.com/translate/auth` 已下线（404），改接 Azure Translator v3 官方接口：订阅密钥存系统凭据库，区域字段可选，服务配置升级到 v2 并自动迁移 v1） | Ad Ni Na | Implemented | P1 |
| SG-TRANS-04 | 简繁转换改用 OpenCC 词组级转换，三平台可用（替代现有系统逐字转换）（按 ADR-0005 实施：OpenCC ver.1.4.2 子模块编译为静态库，四个文本词典随资源分发并在首次使用时解压到 `tdata/serein/opencc-1.4.2`；Linux 也可用；实体与代码片段保持原样） | Na | Implemented | P1 |
| SG-TRANS-05 | 语音转写服务（H02） | Ni | Implemented | P2 |
| SG-TRANS-06 | 系统 AI 草稿预览（H04，macOS） | Ni | Implemented | P2 |
| SG-TRANS-07 | Instant View 与选中文本翻译（聊天中选中文本的“翻译”与消息翻译统一使用所选翻译服务；Instant View 页面翻译待做） | Na | In Progress | P2 |
| SG-TRANS-08 | 按对话自动翻译，话题、对话、账号、全局四级继承（服务设置“非 Premium 用户也可翻译整段对话”默认关闭；开启且所选翻译服务为外部服务或系统翻译时，上游的整段对话翻译（翻译栏、对话菜单“翻译”）对非 Premium 用户可用，跟踪器改用所选服务，外部服务把多条消息的待译片段合并后按服务的单次上限分批请求；Telegram 自己的翻译仍需 Premium；继承关系：全局由上游的翻译按钮与“不翻译的语言”提供；账号级由服务设置“自动翻译对话”提供，按账号保存、默认关闭，开启且本账号可翻译整段对话时，出现翻译提示的对话立即翻译成目标语言，与频道的自动翻译走同一路径；对话级沿用上游按对话的翻译状态，关闭过翻译的对话不会自动翻译；话题共用所在对话的翻译状态，不单独设置） | Ni | Implemented | P3 |
| SG-TRANS-09 | LLM 上下文翻译与摘要（摘要已实现：所选翻译服务是带模型的 OpenAI 兼容或 Anthropic 服务时，对话菜单出现“总结最近的消息”，把最多 60 条最近加载的文字消息以 JSON 数组发送给该服务，按对话的译入语言返回要点，窗口中注明发送对象并可复制；上下文翻译已实现：服务设置“附带前文作为上下文”默认关闭，开启后用这类服务翻译聊天消息时，把该消息之前最多 6 条已加载的文字消息及作者作为仅供参考、不翻译的上下文写入提示词） | Ni | Implemented | P3 |

## NET 网络与代理

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-NET-01 | 代理备注、测速、排序、清理失效节点、导入导出（上游代理管理已提供从剪贴板批量导入、全部删除、分享整个列表、延迟显示与自动切换；服务设置新增“按延迟排序代理”与“移除不可用的代理”，用上游公开的 `MTP::StartProxyCheck` 逐个检测（最长 15 秒），排序时可用代理按延迟在前、未检测的 Web 代理其次、不可用的最后，清理时保留正在使用的代理，检测期间列表若被改动则放弃；服务设置“代理备注”列出当前代理逐个填写备注（单行，至多 64 个字符，至多 200 条），备注按地址与端口保存并显示在上游代理列表的地址之后，钩子为代理行标题拼接处的一行） | Na | Implemented | P2 |
| SG-NET-02 | 代理自动切换与 VPN 感知（上游代理设置已提供按超时自动切换；服务设置“VPN 开启时暂停代理”默认关闭，开启后每 15 秒用不发包的 UDP 连接探测发往 Telegram 数据中心的流量经过哪个网络接口，接口类型或名称表明是 VPN 隧道时关闭代理并提示，VPN 断开后恢复原代理；暂停状态持久保存，重启后仍能恢复；PPPoE 与蜂窝网卡不视为 VPN） | Na | Implemented | P3 |
| SG-NET-03 | 代理订阅（SIP008、Clash 等），新协议只通过外部代理程序接入（服务设置“代理订阅”：填写 HTTPS 地址，“立即更新”下载后提取其中的 tg://proxy、t.me/proxy 与 t.me/socks 链接（最多 200 个，响应不超过 1 MB），经上游链接解析后只加入代理列表中尚未存在的条目并提示数量；SIP008、Clash 等其他协议需要外部代理程序，不在客户端内解析） | Na | Implemented | P3 |
| SG-NET-04 | 自定义 DoH 与 IP 策略（服务设置“自定义 DNS-over-HTTPS 服务器”默认关闭，填写在 `/dns-query` 提供 JSON 接口的主机名后，上游查询备用配置与解析代理域名时先于 Google 与 Cloudflare 请求该服务器；钩子以模板插入上游两处请求列表，不引用上游类型；IP 策略由上游连接设置中的“尝试通过 IPv6 连接”提供） | Na | Implemented | P3 |
| SG-NET-05 | 上传、下载性能档位（先做基准测试） | Na | Planned | P3 |
| SG-NET-06 | 以 Android 客户端身份打开小程序（服务设置“以 Android 客户端身份打开小程序”，默认关闭；开启后申请小程序、主应用、附件菜单与入群验证小程序时向服务器报告 Android 平台，小程序据此提供移动端的功能与布局，JS 桥保持不变） | Aa | Implemented | P3 |

## ACCT 账号

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-ACCT-01 | 放开本地登录账号数量上限（界面设置“允许登录最多 20 个账号”，默认关闭；存储校验上限随之放宽，Premium 宣传中的官方上限不变；账号列表的“添加账号”按钮按 `maxAccounts()` 与官方上限中较大者显示，开启后可加到 20 个） | Na | Implemented | P2 |
| SG-ACCT-02 | 账号作用域设置的双账号隔离验收（自动化部分已覆盖：两个账号的存储互不可见、设备存储拒绝账号选项、设置导出不含账号值、对话列表与过滤、幽灵、历史等按账号保存的选项逐项断言作用域，`Options::Get` 在作用域不符时断言失败；待两个账号同时登录的现场验收） | D | In Progress | P1 |

## ADMIN 群组与资料管理

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-ADMIN-01 | 群组资料页管理快捷入口集合（聊天设置“资料菜单中的群组管理”，默认关闭：在可管理的群组资料菜单中加入成员、管理员、已移除的用户与最近操作，显示条件与上游“管理群组”一致，直接打开上游对应的列表与日志；权限编辑需要复用上游保存逻辑，仍从“管理群组”进入） | Ni | Implemented | P2 |
| SG-ADMIN-02 | 删除群内自己的全部消息、全部解除屏蔽、无成员建群、升级为超级群（“删除我的全部消息”与“升级为超级群组”随聊天设置“管理入口”出现，该设置默认关闭；群组与超级群的对话菜单“删除我的全部消息”：确认后按消息 ID 向前分页搜索自己发出的消息，每页 100 条，交给上游 `Histories::deleteMessages` 为所有人删除，完成后提示删除数量；隐私设置“解除屏蔽所有用户”：确认后按页取回屏蔽列表，逐个经上游 `Api::BlockedPeers` 解除，重复出现或失败即停止并提示已解除数量；无成员建群由上游覆盖：新建群组选成员时不选任何人也能创建；自己创建的基础群资料菜单“升级为超级群”：确认后调用上游 `ApiWrap::migrateChat`，成功后打开新的超级群） | Na | Implemented | P3 |
| SG-ADMIN-03 | 删除对话框的默认勾选项（仍逐次确认）（消息设置“删除”分组，默认全部关闭：删除或清空私聊时默认勾选“同时为对方删除”，群组中同一勾选项代表为所有成员删除，因此不受影响；管理消息时默认勾选“举报垃圾信息”“删除此用户的全部消息”“封禁用户”，经上游 `DefaultModerateMessagesBoxOptions` 覆盖所有删除入口，按住 Ctrl 仍全选；上游删除消息时的“为所有人删除”已有记住选择） | Ni | Implemented | P3 |
| SG-ADMIN-04 | 频道本地别名、彩色管理员头衔（频道本地别名由 SG-PRIV-04 覆盖：本地名称适用于用户、群组与频道；彩色管理员头衔由上游覆盖：消息头部的头衔按群主、管理员与普通成员分别使用 `rankOwnerFg`、`rankAdminFg` 与 `rankUserFg`，群主与管理员的头衔另有同色的半透明底） | Ni | Implemented | P3 |

## SYNC 同步

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-SYNC-01 | 已读状态与消息历史跨设备同步（proto3 协议，自托管服务） | Aa | Planned | P3 |

## TG Telegram 功能完整性

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-TG-01 | Telegram 全部功能保留；Serein 功能全部关闭时行为与上游一致（核心测试 `TestNeutralDefaults` 把 13 个设置页的全部选项注册到一起，逐项要求默认值为关闭、零或空；只有 8 项按写明的理由列入例外：两个空的快捷回复槽位、幽灵模式总开关下的 4 个子项、替代被关闭的官方更新器的 GitHub 更新检查、等于上游尺寸的 100% 贴纸缩放、依赖默认关闭的历史记录的已删除消息淡化；新增非中性默认值或例外失效都会让测试失败；同一测试要求上游的消息菜单项默认显示、Serein 新增的菜单项默认隐藏，只有出现前提本身默认关闭的 6 项例外：编辑历史、已删除消息、不记录此对话依赖默认关闭的消息记录，“标记已读到此处”依赖默认关闭的幽灵模式，两条快捷评价默认未设置；2026-10-01 逐项审计了 Serein 向上游菜单添加入口的全部位置（消息菜单、对话菜单、资料菜单、主菜单、托盘菜单、贴纸包菜单、文件夹菜单与输入框菜单），原先常驻的“跳到开头”“删除我的全部消息”“设置本地名称”“升级为超级群组”、主菜单与托盘的快捷入口、文件夹的“仅显示我管理的群组和频道”、“翻译草稿…”与贴纸包“作者”均改为由默认关闭的选项控制；其余钩子多为读取选项的生成门面，抽查的手写钩子（链接改写、文本预处理、账号上限、排序等）在默认值下走上游路径） | T | Implemented | P0 |
| SG-TG-02 | 每个上游版本（beta 与 stable）同步一次，同步后三平台 CI 通过才发布（同步工具 `tools/serein/upstream_sync.py` 已就绪，流程见 ADR-0001） | T | In Progress | P0 |
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
| 配置、同步与更新 | CORE、PLAT、SYNC |
| 通知呈现 | APPEAR、PRIV |

## 明确排除

- 手机专属交互：振动、滑动手势、底栏、前后摄像头、距离传感器、移动推送。
- 桌面不支持的秘密聊天相关项（截图、屏蔽发起秘密聊天）。
- 冒充官方客户端、使用官方客户端 API 凭据（AyuGram 的做法）。
- 内置 VMess、Shadowsocks、SSR、Trojan 代理核心；OpenKeychain 集成。
- 运行时切换测试服务器（仅作为开发构建配置）。
