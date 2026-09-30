# 功能矩阵（TODO 清单）

本表是 SereinGram 功能状态的唯一来源，状态与优先级定义见 [产品规格](README.md#3-标识与状态)。

来源缩写：`Ni` Nagram iOS · `Na` Nagram Android（含 NekoX／Nekogram 继承项）· `Ad` AyuGram Desktop · `Aa` AyuGram Android · `T` Telegram Desktop 已有 · `D` 桌面补充。

现有代码来自 Nagram-qt 的 M0–M7（设置条目编号 A02–J04，见 `docs/nagram/settings-page.md`）。这些条目代码已完成、`test_nagram` 通过，但多数缺少现场验收，统一记为 `Implemented`；逐项的现场证据与缺口见 `docs/nagram/design.md` 第 8 节。

核对口径：

- AyuGram：`docs.ayugram.one/desktop` 功能列表、AyuGramDesktop 与 AyuGram4A 的 README（2026-09-30 抓取）。
- 手机版 Nagram：`docs/nagram/feature-catalog.md` 逐项目录（iOS 集中偏好、Android 继承与增强偏好、菜单动作、无开关能力）与 Nagram Android README。目录中“纳入／合并”的条目按下文“需求族映射”归入本表各族，排除项沿用目录末节口径。

## PLAT 平台与交付

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-PLAT-01 | macOS 通用二进制（arm64 + x86_64）构建与 DMG（依赖与应用默认按 `x86_64;arm64` 构建；CI 用 `lipo -verify_arch` 校验两种架构后打包 DMG 上传；签名与公证待做） | T | In Progress | P0 |
| SG-PLAT-02 | Windows x64 构建、安装包与便携版（便携版为单个 `SereinGram.exe`；CI 以 Inno Setup 编译 `setup.iss` 生成安装包，步骤暂为允许失败的试验状态；签名待做） | T | In Progress | P0 |
| SG-PLAT-03 | Linux x86_64 静态构建（Rocky Linux 8 容器）与 tar 包（CI 打包 `SereinGram.tar.gz` 上传） | T | In Progress | P0 |
| SG-PLAT-04 | 三平台 CI：构建、`test_serein`、全部守卫 | D | Planned | P0 |
| SG-PLAT-05 | 发行版打包：Flatpak 清单、AUR PKGBUILD、`DESKTOP_APP_USE_PACKAGED` 依赖清单 | Ad | Planned | P2 |
| SG-PLAT-06 | Windows arm64 构建（暂从 CI 矩阵移除：上游 ffmpeg n8.1.3 在 arm64 上生成的 `epel_neon.d` 依赖文件格式错误，导致依赖构建失败） | T | Planned | P2 |
| SG-PLAT-07 | 独立更新检查（GitHub Releases，默认不自动下载）（界面设置“在 GitHub 上检查更新”默认开启：启动 30 秒后及每 24 小时查询最新正式版，只接受 github.com 的发布页链接，发现新版本时提示链接，不自动下载；官方更新通道在构建中关闭；推送 `v*` 标签时三平台工作流把产物上传到草稿预发布 Release，由维护者核对后发布） | D | In Progress | P2 |
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
| SG-CORE-01 | proto3 schema 作为设置、结构化配置、导出包与历史记录的唯一声明来源；Buf lint 与 breaking 检查（12 个设置 proto 生成 134 个选项，10 个配置与导出格式 proto；菜单配置、重复发送确认、消息过滤、链接规则与消息截图已由设置 proto 以自定义界面字段声明，`buf breaking` 可发现键名改动。仍在 C++ 中手写的 3 个选项及原因：本地别名的校验需要应用层的对话 ID 类型判断，核心测试无法链接；服务配置与“优先系统 AI”被上游文件直接引用，迁移会增加上游改动） | D | In Progress | P0 |
| SG-CORE-02 | 代码生成器：注册表元数据、C++ 值类型、JSON 编解码、校验（均已生成；既有结构化配置已迁移 10 个：链接规则、快捷回复、消息过滤、主菜单、消息菜单、翻译与转写服务、历史排除列表、本地别名、贴纸目录导出文件、消息截图设置；所有存储与导出格式都由 proto 声明；过滤、链接规则与主菜单的设置界面已改用生成的结构体读写；翻译与转写服务的设置界面仍按键名读写已校验的 JSON；支持字符串键的 map 字段、double 与可空字段） | D | In Progress | P0 |
| SG-CORE-03 | 存储端口与适配器：设备偏好、账号偏好、历史库（选项存储分设备与账号两个作用域；历史库经 `Ports::HistoryStore` 端口由 Qt SQL 适配器实现，记录整体用基于本地密钥派生的 AES-GCM 加密，缓存媒体副本用同一密钥加密） | D | Implemented | P0 |
| SG-CORE-04 | 上游挂钩门面 `serein/hooks`，上游文件只调用门面（设置选项的门面由 proto 生成到 `serein/hooks/gen`，面向上游的薄接口头文件已移入 `serein/hooks/<领域>/`，并可脱离应用代码单独通过语法检查；直接包含内部头文件的上游文件已从 87 降到 0，并由预算守卫锁定；依赖上游嵌套类型的门面声明为函数模板，在实现文件中对该类型显式实例化，门面本身不包含应用代码） | D | Implemented | P0 |
| SG-CORE-05 | 功能模块注册与生命周期（应用、会话、窗口作用域）：应用与会话作用域已由 `serein/app/modules.cpp` 模块表统一分发（对话排序已从上游窗口构造函数移到会话作用域）；窗口作用域由 `Serein::Hooks::OnWindowStarted` 分发，首个使用者是最近会话记录；主菜单条目统一由 `Serein::Hooks::FillMainMenu` 添加 | D | Implemented | P0 |
| SG-CORE-06 | 设置页与搜索索引由 schema 元数据生成（开关、选项、数值、文本、子页）：开关行、小标题、说明、依赖开关与自定义行位置已由 proto 生成 `AddLayout`，界面、聊天、消息、写作、菜单、媒体、隐私（含幽灵与历史）七页已迁移；数值输入行（范围取自 gte/lte 规则，0 的文字与复数格式由 `number` 选项声明）已生成；单选行（按序语言键或 `in` 规则加后缀）已生成；文本行（`text` 选项声明占位语言键，未设置时行标签显示占位文字，`disabled_by` 使该行在对应开关打开时隐藏）已生成，“已编辑/已删除标记文字”已改用生成行；子页待做 | D | In Progress | P0 |
| SG-CORE-07 | 英文、简体、繁体内置文案与一致性检查 | Ni Na | Implemented | P0 |
| SG-CORE-08 | 配置管理：已修改项、导出、导入差异预览、诊断信息（J01–J04） | Ni Na | Implemented | P1 |
| SG-CORE-09 | 守卫：源文件 ≤ 1000 行、模块依赖方向、上游侵入预算、生成代码漂移 | D | Implemented | P0 |
| SG-CORE-10 | 测试：纯逻辑单元测试与 `-testagent` 界面场景 | D | Implemented | P0 |
| SG-CORE-11 | 更多界面语言的社区翻译平台接入 | Ad Na | Planned | P3 |

## GHOST 幽灵模式

策略模型与按账号保存的设置（`proto/serein/settings/v1/ghost.proto`、`serein/features/ghost/model/`）已实现并有测试；上游挂钩与设置界面待接入。

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-GHOST-01 | 不发送消息已读回执（私聊、群组、频道、讨论、话题、提及与反应已读）（对话、话题与评论、频道私信子列表与内容已读均已拦截，本地已读状态照常推进；“全部提及已读”“全部回应已读”只清除自己的计数，不拦截） | Ad Aa Na | In Progress | P1 |
| SG-GHOST-02 | 不发送动态已读与动态浏览 | Ad Aa Na | In Progress | P1 |
| SG-GHOST-03 | 不发送在线状态；发送消息后立即恢复离线 | Ad Aa Na | In Progress | P1 |
| SG-GHOST-04 | 不发送输入、上传、选贴纸等活动状态 | Ad Aa Na | In Progress | P1 |
| SG-GHOST-05 | 总开关、子项锁定，全局策略与按账号策略 | Ad Na | In Progress | P1 |
| SG-GHOST-06 | 主菜单、托盘与聊天顶部的快速切换入口和状态指示 | Ad Na | In Progress | P1 |
| SG-GHOST-07 | 阅读频道消息时不增加浏览数 | Ad | In Progress | P2 |
| SG-GHOST-08 | 发送消息或互动后自动标记该对话已读（可选） | Ad Na | In Progress | P2 |
| SG-GHOST-09 | 幽灵模式下用定时消息发送，避免上线（在 `Api::SendAction` 构造处统一挂钩：未手动定时、非快捷消息、非收藏夹时改为 12 秒后定时发送） | Ad | In Progress | P2 |
| SG-GHOST-10 | “仅本地已读”与“同步到服务端”两个显式动作：读到此处、全部已读（消息菜单“读到此处”已实现：幽灵模式隐藏已读时，把已读同步到该条为止；对话列表的“标记为已读”“全部标记为已读”默认只在本地生效，开启“手动标记已读时发送已读回执”后同步到服务端） | Ad Na | Implemented | P2 |
| SG-GHOST-11 | 查看动态前提示当前幽灵模式状态 | Na | Planned | P3 |

## HIST 消息历史与防撤回

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-HIST-01 | 保存已接收的删除消息并在原位显示，附删除标记（防撤回）：本次会话内原位保留与底部“已删除”标记已实现（默认关闭）；重启后每次加载消息分片时，从历史库取出该范围内的删除记录，以本地消息按时间插回原位（文字与格式保留，媒体显示为摘要文字，不含反应与回复关系） | Ad Aa Na | Implemented | P1 |
| SG-HIST-02 | 保存编辑历史，在消息菜单查看各版本 | Ad Aa Na | Implemented | P1 |
| SG-HIST-03 | 历史库加密存储（账号本地密钥派生），退出账号时清理（AES-256-GCM 密钥由账号本地密钥派生；退出账号后同一账号槽位的本地密钥会保留，因此在会话结束且不是退出应用时删除历史库及其日志文件） | D | Implemented | P1 |
| SG-HIST-04 | 保留期限、容量上限、单对话清理与全部清理（保留天数与条数上限有设置行，会话启动时清理；“已删除消息”窗口可清除单个对话的记录，隐私页“清除已保存的消息历史”清除整个账号，两者都需确认） | Na | Implemented | P1 |
| SG-HIST-05 | 自定义已删除、已编辑标记文字；已删除消息半透明（消息页“标记与计数”下的“已编辑标记文字”“已删除标记文字”已实现，最长 64 字符；“淡化已删除消息”默认开启，原位保留的已删除消息以 60% 不透明度绘制） | Aa Na | Implemented | P2 |
| SG-HIST-06 | 按对话浏览已删除消息（对话菜单与消息菜单的“已删除消息”列出该对话最近 100 条，含发送者与发送时间；以聊天气泡形式展示与媒体预览待做） | Ad Aa | In Progress | P2 |
| SG-HIST-07 | 是否记录机器人消息；按对话排除（“记录机器人消息”为隐私页开关；消息菜单“不记录此对话的历史”按账号维护排除列表，列表格式由 `config/v1/history_exclusions.proto` 声明） | Na | Implemented | P2 |
| SG-HIST-08 | 已下载媒体不随删除清除，历史中可继续打开（删除时记录已下载到磁盘的文件路径，“已删除消息”中文件仍在时显示“打开文件”；未下载、但已完整载入内存的媒体（看过的图片、语音、贴纸与小文件，单个不超过 20 MB）用历史库的密钥加密另存到账号目录的 `serein_media`，“打开媒体”解密到临时目录后交给系统程序打开；清除对话、退出登录与过期清理时一并删除，流式播放未完整下载的视频不在此列） | Ad Aa | Implemented | P2 |
| SG-HIST-09 | 限时图片、视频过期后仍可查看（隐私页“保留已过期的限时媒体”：到期时不清除缓存、不替换为“已过期”，本次会话内可反复打开；重启后按服务端状态显示。自动删除计时到期的消息按“原位保留已删除消息”处理并写入历史库） | Ad Aa | Implemented | P2 |
| SG-HIST-10 | 保留被移出或封禁的群组、频道的本地记录 | Aa | Planned | P3 |

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
| SG-FILTER-08 | 全局、账号、对话、话题四级规则继承与远程规则源 | Ni | Planned | P3 |

## PRIV 隐私与本地能力

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-PRIV-01 | 主播模式：截屏录屏排除窗口，遮盖会话列表、标题与通知（G02） | Ad Ni | Implemented | P1 |
| SG-PRIV-02 | 主播模式快捷键与菜单、托盘入口（主菜单开关与托盘菜单“开启/关闭演示模式”已接入，托盘项由 `serein/app/tray_menu.cpp` 提供；快捷键命令 `serein_toggle_presentation_mode` 与 `serein_toggle_ghost_mode` 默认不绑定按键，可在 `shortcuts-custom.json` 中绑定，切换后提示当前状态） | Ad | Implemented | P1 |
| SG-PRIV-03 | 遮盖本机手机号（G01） | Ni Na | Implemented | P1 |
| SG-PRIV-04 | 本地备注名称 | Ni | Implemented | P2 |
| SG-PRIV-05 | 默认隐藏赞助消息与代理赞助频道（B10、B11） | Ad Ni | Implemented | P1 |
| SG-PRIV-06 | 隐藏已读时间提示与分享手机号提示（G03、G04） | Ni | Implemented | P2 |
| SG-PRIV-07 | 本地 Premium 外观，仅本地显示，不伪造服务端权益 | Ad Aa | Planned | P3 |
| SG-PRIV-08 | 受保护内容的本地复制与保存（复制文字、保存媒体与截图已放开，转发仍由服务端拒绝；受保护的动态可截图，并与普通动态一样可由 Premium 用户保存，不绕过 Premium 限制） | Ad Na | Implemented | P2 |
| SG-PRIV-09 | 设置锁与本地账号隐藏 | Ni | Planned | P3 |
| SG-PRIV-10 | 检测到录屏软件时自动开启主播模式 | Ad | Planned | P3 |

## APPEAR 界面与外观

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-APPEAR-01 | 气泡与头像圆角（A02–A04） | Ad Ni | Implemented | P1 |
| SG-APPEAR-02 | 消息宽度、频道宽消息、隐藏气泡尾巴、引用配色、回复缩略图、忽略对话主题（A05–A10） | Ad Ni | Implemented | P1 |
| SG-APPEAR-03 | 主菜单标题、顺序、显隐与节日装饰（A11） | Ni | Implemented | P1 |
| SG-APPEAR-04 | 应用图标角标、通知延迟（A12–A14） | Ni | Implemented | P2 |
| SG-APPEAR-05 | 界面半角标点（A15） | Ni | Implemented | P2 |
| SG-APPEAR-06 | 主字体与等宽字体自定义（主字体由上游 `customFontFamily` 提供；等宽字体的字体族写死在 lib_ui，需上游支持后接入） | Ad Ni | In Progress | P2 |
| SG-APPEAR-07 | 应用图标选择 | Ad Ni | Planned | P2 |
| SG-APPEAR-08 | 以频道身份发言时显示频道徽标（消息页开关，默认开启：超级群中以广播频道身份发送的消息在名字右侧显示上游已有的“频道”标记；匿名管理员以群身份发言时不显示） | Ad Ni | Implemented | P2 |
| SG-APPEAR-09 | 连续的文字、贴纸、圆形视频消息组的气泡尾巴修正 | Ad | Planned | P3 |
| SG-APPEAR-10 | 圆角贴纸 | Ad | Planned | P3 |
| SG-APPEAR-11 | Material 风格开关动画 | Ad | Planned | P3 |
| SG-APPEAR-12 | 通知位置新增顶部居中 | Ad | Planned | P3 |
| SG-APPEAR-13 | macOS 触感反馈、Force Touch 媒体预览与快速反应 | Ad | Planned | P3 |

## CHATS 会话列表与导航

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-CHATS-01 | 紧凑列表、预览行数、隐藏预览、隐藏动态（B01–B04） | Ni Na | Implemented | P1 |
| SG-CHATS-02 | 启动文件夹、隐藏“全部会话”、文件夹归档入口、隐藏文件夹未读数（B05–B08） | Ad Ni | Implemented | P1 |
| SG-CHATS-03 | 会话排序规则（B09） | Na | Implemented | P2 |
| SG-CHATS-04 | 隐藏 Premium 推广与生日提示（B12、B13） | Ni | Implemented | P2 |
| SG-CHATS-05 | 滚动到底不切换频道或话题（B14、B15） | Ni | Implemented | P2 |
| SG-CHATS-06 | 文件夹属性“仅显示我管理的群组和频道” | Ni | Implemented | P2 |
| SG-CHATS-07 | 一键已读全部对话或当前文件夹（由上游提供：对话列表与文件夹菜单的“标记为已读”“全部对话标记为已读”，`menu/menu_mark_as_read.cpp`，核对于 2026-09-30） | Ad Na | Implemented | P2 |
| SG-CHATS-08 | 跳到对话开头（聊天窗口菜单“跳到对话开头”，由 `serein/app/peer_menu.cpp` 提供；话题内跳到话题的首条消息） | Ad Ni | Implemented | P2 |
| SG-CHATS-09 | 最近会话列表（按账号记录最近打开的 30 个对话；主菜单“最近会话”与快捷键命令 `serein_recent_chats` 打开列表，点击进入对话，可清空） | Ni | Implemented | P2 |
| SG-CHATS-10 | 聊天顶部工具栏：搜索、媒体、置顶、跳到开头、静音、清缓存 | Ni | Planned | P2 |
| SG-CHATS-11 | 保存并恢复阅读位置 | Ni | Planned | P2 |
| SG-CHATS-12 | 本地置顶扩展 | Ni | Planned | P3 |

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
| SG-MSG-12 | 注册日期估算（标注估算来源）、波斯日历（隐私页“显示估算的注册时间”，默认关闭：按用户 ID 在 WizardLoop/CreationDate（MIT，提交 f37728802d36，许可证随 `Telegram/Resources/serein/regdate_points.LICENSE` 分发）的 212 个公开数据点之间线性插值，资料页显示“约某年某月”，晚于最后数据点时显示“晚于”；波斯日历待做） | Ni Na | In Progress | P3 |

## COMPOSE 输入与发送

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-COMPOSE-01 | 输入框按钮显隐（D01–D11） | Ad Ni | Implemented | P1 |
| SG-COMPOSE-02 | 悬停不弹出面板、机器人命令先填入、占位文字（D12–D15） | Ni | Implemented | P2 |
| SG-COMPOSE-03 | 关闭自动 Markdown、默认不显示链接预览、发送与编辑时中西文加空格、默认代码语言、快捷回复（D16–D21） | Ni Na | Implemented | P1 |
| SG-COMPOSE-04 | 贴纸、GIF、语音、圆形视频、通话前确认（D22–D26） | Ni Na | Implemented | P1 |
| SG-COMPOSE-05 | 先转发后附言（D27） | Na | Implemented | P2 |
| SG-COMPOSE-06 | 草稿翻译与系统 AI 草稿（H04） | Ni | Implemented | P2 |
| SG-COMPOSE-07 | 静音发送策略：从不、预设、始终（“按对话”由上游的静音发送开关提供；写作页新增“总是静音发送”，在 `Api::SendAction` 的统一挂钩中生效） | Ni | Implemented | P2 |
| SG-COMPOSE-08 | 文本替换规则 | Na | Planned | P3 |
| SG-COMPOSE-09 | 格式工具栏 | Ni | Planned | P3 |

## MENU 消息菜单

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-MENU-01 | 上游菜单项三态显隐：显示、隐藏、按住 Option／Alt 显示（E01–E14） | Ad Ni Na | Implemented | P1 |
| SG-MENU-02 | 复读、无引用复读、无引用转发，复读确认（E15–E17、E24） | Ni Na | Implemented | P1 |
| SG-MENU-03 | 合并、反序、去署名后填入草稿，批量存入收藏夹；选择此人的消息（E18、E19） | Ni Na | Implemented | P2 |
| SG-MENU-04 | 媒体信息、消息截图、阅读转换切换（E20–E22） | Ad Ni | Implemented | P2 |
| SG-MENU-05 | 消息截图的简化引用样式 | Ni | Planned | P3 |
| SG-MENU-06 | 查看编辑历史、查看已删除内容（随 SG-HIST-01／02） | Ad Aa | Implemented | P1 |
| SG-MENU-07 | 读到此处、复制回调数据、消息详情等 AyuGram 菜单项及其显隐（“读到此处”已实现，可在菜单设置中隐藏；“按钮数据”“消息详情”已实现） | Ad | Implemented | P2 |
| SG-MENU-08 | 区间选择、批量取消置顶、快捷评价文本、文本菜单“提及”（“选择区间”“取消置顶所选消息”已实现；输入框中选中文字后右键“提及…”，输入用户名、t.me 链接或本账号已知的用户 ID，把选中文字变成对该用户的提及，写作设置“在文本菜单中显示‘提及…’”默认开启；菜单设置“快捷评价”可填写两条文本，设置后消息菜单在“回复”下方出现“回复‘…’”，点击即以该文本回复这条消息，不会自动发送） | Na | Implemented | P2 |

## MEDIA 媒体与贴纸

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-MEDIA-01 | 贴纸大小、隐藏时间、最近使用数量（最多 200）、隐藏群组与推荐贴纸、问候贴纸（F01–F08） | Ad Ni | Implemented | P1 |
| SG-MEDIA-02 | 关闭视频自动播放、GIF 播放控制（F09、F10） | Ad Ni | Implemented | P1 |
| SG-MEDIA-03 | 以文件发送的 MP4 保留视频预览（F11） | Ni | Implemented | P2 |
| SG-MEDIA-04 | 贴纸包列表导出与导入（F12、F13） | Na | Implemented | P2 |
| SG-MEDIA-05 | 查询贴纸包、表情包作者并打开资料（贴纸包与表情包窗口右上角菜单“作者”：由贴纸包 ID 推算作者用户 ID，本地已知该用户时打开资料页，否则复制 ID；按 ID 远程解析未知用户待做） | Ad | In Progress | P2 |
| SG-MEDIA-06 | 下载时保留原始文件名（先核对上游现状）（由上游提供：`DocumentFileNameForSave` 默认使用原始文件名，核对于 2026-09-30） | Na | Implemented | P2 |
| SG-MEDIA-07 | 不经贴纸包收藏单个贴纸、收藏去重 | Na | Planned | P3 |
| SG-MEDIA-08 | 通话与录音降噪、语音增强 | Na | Planned | P3 |
| SG-MEDIA-09 | 自定义表情资源包 | Na | Planned | P3 |
| SG-MEDIA-10 | 音乐封面服务与已保存音乐 | Ad Ni | Planned | P3 |

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
| SG-TRANS-08 | 按对话自动翻译，话题、对话、账号、全局四级继承 | Ni | Planned | P3 |
| SG-TRANS-09 | LLM 上下文翻译与摘要 | Ni | Planned | P3 |

## NET 网络与代理

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-NET-01 | 代理备注、测速、排序、清理失效节点、导入导出（上游代理管理已提供从剪贴板批量导入、全部删除、分享整个列表、延迟显示与自动切换；备注、按延迟排序、清理失效节点需要改动上游的私有列表与行控件，待做） | Na | Planned | P2 |
| SG-NET-02 | 代理自动切换与 VPN 感知（上游代理设置已提供按超时自动切换；VPN 感知待做） | Na | In Progress | P3 |
| SG-NET-03 | 代理订阅（SIP008、Clash 等），新协议只通过外部代理程序接入 | Na | Planned | P3 |
| SG-NET-04 | 自定义 DoH 与 IP 策略 | Na | Planned | P3 |
| SG-NET-05 | 上传、下载性能档位（先做基准测试） | Na | Planned | P3 |

## ACCT 账号

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-ACCT-01 | 放开本地登录账号数量上限（界面设置“允许登录最多 20 个账号”，默认关闭；存储校验上限随之放宽，Premium 宣传中的官方上限不变） | Na | In Progress | P2 |
| SG-ACCT-02 | 账号作用域设置的双账号隔离验收（自动化部分已覆盖：两个账号的存储互不可见、设备存储拒绝账号选项、设置导出不含账号值、对话列表与过滤、幽灵、历史等按账号保存的选项逐项断言作用域，`Options::Get` 在作用域不符时断言失败；待两个账号同时登录的现场验收） | D | In Progress | P1 |

## ADMIN 群组与资料管理

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-ADMIN-01 | 群组资料页管理快捷入口集合 | Ni | Planned | P2 |
| SG-ADMIN-02 | 删除群内自己的全部消息、全部解除屏蔽、无成员建群、升级为超级群 | Na | Planned | P3 |
| SG-ADMIN-03 | 删除对话框的默认勾选项（仍逐次确认） | Ni | Planned | P3 |
| SG-ADMIN-04 | 频道本地别名、彩色管理员头衔 | Ni | Planned | P3 |

## SYNC 同步

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-SYNC-01 | 已读状态与消息历史跨设备同步（proto3 协议，自托管服务） | Aa | Planned | P3 |

## TG Telegram 功能完整性

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-TG-01 | Telegram 全部功能保留；Serein 功能全部关闭时行为与上游一致 | T | In Progress | P0 |
| SG-TG-02 | 每个上游版本（beta 与 stable）同步一次，同步后三平台 CI 通过才发布（同步工具 `tools/serein/upstream_sync.py` 已就绪，流程见 ADR-0001） | T | In Progress | P0 |
| SG-TG-03 | 上游侵入预算：上游文件数、新增行数、挂钩数在 CI 中统计并设上限 | D | Implemented | P0 |

## 需求族映射

手机版 Nagram 需求（`docs/nagram/requirements.md` F01–F18）与本表各族的对应关系，用于逐项核对覆盖率：

| Nagram 需求族 | 本表 |
| --- | --- |
| F01 品牌、主题与外观 | APPEAR、BRAND |
| F02 聊天列表与导航 | CHATS |
| F03 消息显示 | MSG |
| F04 输入与文本 | COMPOSE |
| F05 消息菜单与批量操作 | MENU |
| F06 媒体与贴纸 | MEDIA |
| F07 翻译与 LLM | TRANS |
| F08 语音转写 | TRANS |
| F09 过滤与规则 | FILTER |
| F10 隐私与本地锁 | PRIV |
| F11 回执与在线状态 | GHOST |
| F12 本地历史 | HIST |
| F13 消息截图 | MENU |
| F14 资料与群管理 | MSG、ADMIN |
| F15 网络与代理 | NET |
| F16 链接与外部集成 | FILTER、TRANS |
| F17 配置、同步与更新 | CORE、PLAT、SYNC |
| F18 通知呈现 | APPEAR、PRIV |

## 明确排除

- 手机专属交互：振动、滑动手势、底栏、前后摄像头、距离传感器、移动推送。
- 桌面不支持的秘密聊天相关项（截图、屏蔽发起秘密聊天）。
- 冒充官方客户端、使用官方客户端 API 凭据（AyuGram 的做法）。
- 内置 VMess、Shadowsocks、SSR、Trojan 代理核心；OpenKeychain 集成。
- 运行时切换测试服务器（仅作为开发构建配置）。
