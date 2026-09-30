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
| SG-PLAT-01 | macOS 通用二进制（arm64 + x86_64）构建与 DMG | T | Planned | P0 |
| SG-PLAT-02 | Windows x64 构建、安装包与便携版 | T | Planned | P0 |
| SG-PLAT-03 | Linux x86_64 静态构建（Rocky Linux 8 容器）与 tar 包 | T | Planned | P0 |
| SG-PLAT-04 | 三平台 CI：构建、`test_serein`、全部守卫 | D | Planned | P0 |
| SG-PLAT-05 | 发行版打包：Flatpak 清单、AUR PKGBUILD、`DESKTOP_APP_USE_PACKAGED` 依赖清单 | Ad | Planned | P2 |
| SG-PLAT-06 | Windows arm64 构建 | T | Planned | P2 |
| SG-PLAT-07 | 独立更新检查（GitHub Releases，默认不自动下载） | D | Planned | P2 |
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
| SG-CORE-01 | proto3 schema 作为设置、结构化配置、导出包与历史记录的唯一声明来源；Buf lint 与 breaking 检查 | D | Planned | P0 |
| SG-CORE-02 | 代码生成器：注册表元数据、C++ 值类型、JSON 编解码、校验 | D | Planned | P0 |
| SG-CORE-03 | 存储端口与适配器：设备偏好、账号偏好、历史库 | D | In Progress | P0 |
| SG-CORE-04 | 上游挂钩门面 `serein/hooks`，上游文件只调用门面 | D | Planned | P0 |
| SG-CORE-05 | 功能模块注册与生命周期（应用、会话、窗口作用域） | D | Planned | P0 |
| SG-CORE-06 | 设置页与搜索索引由 schema 元数据生成（开关、选项、数值、文本、子页） | D | In Progress | P0 |
| SG-CORE-07 | 英文、简体、繁体内置文案与一致性检查 | Ni Na | Implemented | P0 |
| SG-CORE-08 | 配置管理：已修改项、导出、导入差异预览、诊断信息（J01–J04） | Ni Na | Implemented | P1 |
| SG-CORE-09 | 守卫：源文件 ≤ 1000 行、模块依赖方向、上游侵入预算、生成代码漂移 | D | In Progress | P0 |
| SG-CORE-10 | 测试：纯逻辑单元测试与 `-testagent` 界面场景 | D | Implemented | P0 |
| SG-CORE-11 | 更多界面语言的社区翻译平台接入 | Ad Na | Planned | P3 |

## GHOST 幽灵模式

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-GHOST-01 | 不发送消息已读回执（私聊、群组、频道、讨论、话题、提及与反应已读） | Ad Aa Na | Planned | P1 |
| SG-GHOST-02 | 不发送动态已读与动态浏览 | Ad Aa Na | Planned | P1 |
| SG-GHOST-03 | 不发送在线状态；发送消息后立即恢复离线 | Ad Aa Na | Planned | P1 |
| SG-GHOST-04 | 不发送输入、上传、选贴纸等活动状态 | Ad Aa Na | Planned | P1 |
| SG-GHOST-05 | 总开关、子项锁定，全局策略与按账号策略 | Ad Na | Planned | P1 |
| SG-GHOST-06 | 主菜单、托盘与聊天顶部的快速切换入口和状态指示 | Ad Na | Planned | P1 |
| SG-GHOST-07 | 阅读频道消息时不增加浏览数 | Ad | Planned | P2 |
| SG-GHOST-08 | 发送消息或互动后自动标记该对话已读（可选） | Ad Na | Planned | P2 |
| SG-GHOST-09 | 幽灵模式下用定时消息发送，避免上线 | Ad | Planned | P2 |
| SG-GHOST-10 | “仅本地已读”与“同步到服务端”两个显式动作：读到此处、全部已读 | Ad Na | Planned | P2 |
| SG-GHOST-11 | 查看动态前提示当前幽灵模式状态 | Na | Planned | P3 |

## HIST 消息历史与防撤回

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-HIST-01 | 保存已接收的删除消息并在原位显示，附删除标记（防撤回） | Ad Aa Na | Planned | P1 |
| SG-HIST-02 | 保存编辑历史，在消息菜单查看各版本 | Ad Aa Na | Planned | P1 |
| SG-HIST-03 | 历史库加密存储（账号本地密钥派生），退出账号时按设置清理 | D | Planned | P1 |
| SG-HIST-04 | 保留期限、容量上限、单对话清理与全部清理 | Na | Planned | P1 |
| SG-HIST-05 | 自定义已删除、已编辑标记文字；已删除消息半透明 | Aa Na | In Progress | P2 |
| SG-HIST-06 | 按对话浏览已删除消息 | Ad Aa | Planned | P2 |
| SG-HIST-07 | 是否记录机器人消息；按对话排除 | Na | Planned | P2 |
| SG-HIST-08 | 已下载媒体不随删除清除，历史中可继续打开 | Ad Aa | Planned | P2 |
| SG-HIST-09 | 限时图片、视频过期后仍可查看 | Ad Aa | Planned | P2 |
| SG-HIST-10 | 保留被移出或封禁的群组、频道的本地记录 | Aa | Planned | P3 |

SG-HIST-05 的“已编辑”标记文字已由 C09 实现，删除标记随 SG-HIST-01 落地。

## FILTER 过滤与规则

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-FILTER-01 | 正则过滤：遮盖、替换、隐藏，排除对话，规则测试框，导入导出（I01） | Ad Aa Ni Na | Implemented | P1 |
| SG-FILTER-02 | 隐藏已屏蔽用户的消息（I01 子项） | Ad Na | Implemented | P1 |
| SG-FILTER-03 | 在回复引用、反应列表、成员列表中也隐藏已屏蔽用户 | Ad | Planned | P2 |
| SG-FILTER-04 | 按对话的过滤规则与共享规则列表导入 | Ad | Planned | P2 |
| SG-FILTER-05 | Zalgo 字符过滤 | Ni | Implemented | P2 |
| SG-FILTER-06 | 消息菜单“隐藏此人的消息”（E23） | Ni | Implemented | P2 |
| SG-FILTER-07 | 链接规则：URL 修正、参数清理、打开前确认（I02） | Ni | Implemented | P2 |
| SG-FILTER-08 | 全局、账号、对话、话题四级规则继承与远程规则源 | Ni | Planned | P3 |

## PRIV 隐私与本地能力

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-PRIV-01 | 主播模式：截屏录屏排除窗口，遮盖会话列表、标题与通知（G02） | Ad Ni | Implemented | P1 |
| SG-PRIV-02 | 主播模式快捷键与菜单、托盘入口 | Ad | Planned | P1 |
| SG-PRIV-03 | 遮盖本机手机号（G01） | Ni Na | Implemented | P1 |
| SG-PRIV-04 | 本地备注名称 | Ni | Implemented | P2 |
| SG-PRIV-05 | 默认隐藏赞助消息与代理赞助频道（B10、B11） | Ad Ni | Implemented | P1 |
| SG-PRIV-06 | 隐藏已读时间提示与分享手机号提示（G03、G04） | Ni | Implemented | P2 |
| SG-PRIV-07 | 本地 Premium 外观，仅本地显示，不伪造服务端权益 | Ad Aa | Planned | P3 |
| SG-PRIV-08 | 受保护内容的本地复制与保存 | Ad Na | Planned | P2 |
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
| SG-APPEAR-06 | 主字体与等宽字体自定义 | Ad Ni | Planned | P2 |
| SG-APPEAR-07 | 应用图标选择 | Ad Ni | Planned | P2 |
| SG-APPEAR-08 | 以频道身份发言时显示频道徽标 | Ad Ni | Planned | P2 |
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
| SG-CHATS-07 | 一键已读全部对话或当前文件夹 | Ad Na | Planned | P2 |
| SG-CHATS-08 | 跳到对话开头 | Ad Ni | Planned | P2 |
| SG-CHATS-09 | 最近会话列表 | Ni | Planned | P2 |
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
| SG-MSG-08 | 消息详情：日期、转发来源、贴纸包与表情包作者 | Ad Na | Planned | P2 |
| SG-MSG-09 | 内联按钮回调数据查看与复制 | Ad | Planned | P2 |
| SG-MSG-10 | 语音与圆形视频拖动进度（先核对上游现状） | Ad | Planned | P2 |
| SG-MSG-11 | 反应时间显示秒 | Ad | Planned | P3 |
| SG-MSG-12 | 注册日期估算（标注估算来源）、波斯日历 | Ni Na | Planned | P3 |

## COMPOSE 输入与发送

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-COMPOSE-01 | 输入框按钮显隐（D01–D11） | Ad Ni | Implemented | P1 |
| SG-COMPOSE-02 | 悬停不弹出面板、机器人命令先填入、占位文字（D12–D15） | Ni | Implemented | P2 |
| SG-COMPOSE-03 | 关闭自动 Markdown、默认不显示链接预览、发送与编辑时中西文加空格、默认代码语言、快捷回复（D16–D21） | Ni Na | Implemented | P1 |
| SG-COMPOSE-04 | 贴纸、GIF、语音、圆形视频、通话前确认（D22–D26） | Ni Na | Implemented | P1 |
| SG-COMPOSE-05 | 先转发后附言（D27） | Na | Implemented | P2 |
| SG-COMPOSE-06 | 草稿翻译与系统 AI 草稿（H04） | Ni | Implemented | P2 |
| SG-COMPOSE-07 | 静音发送策略：从不、预设、始终 | Ni | Planned | P2 |
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
| SG-MENU-06 | 查看编辑历史、查看已删除内容（随 SG-HIST-01／02） | Ad Aa | Planned | P1 |
| SG-MENU-07 | 读到此处、复制回调数据、消息详情等 AyuGram 菜单项及其显隐 | Ad | Planned | P2 |
| SG-MENU-08 | 区间选择、批量取消置顶、快捷评价文本、提及时附带 @用户名 | Na | Planned | P2 |

## MEDIA 媒体与贴纸

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-MEDIA-01 | 贴纸大小、隐藏时间、最近使用数量（最多 200）、隐藏群组与推荐贴纸、问候贴纸（F01–F08） | Ad Ni | Implemented | P1 |
| SG-MEDIA-02 | 关闭视频自动播放、GIF 播放控制（F09、F10） | Ad Ni | Implemented | P1 |
| SG-MEDIA-03 | 以文件发送的 MP4 保留视频预览（F11） | Ni | Implemented | P2 |
| SG-MEDIA-04 | 贴纸包列表导出与导入（F12、F13） | Na | Implemented | P2 |
| SG-MEDIA-05 | 查询贴纸包、表情包作者并打开资料 | Ad | Planned | P2 |
| SG-MEDIA-06 | 下载时保留原始文件名（先核对上游现状） | Na | Planned | P2 |
| SG-MEDIA-07 | 不经贴纸包收藏单个贴纸、收藏去重 | Na | Planned | P3 |
| SG-MEDIA-08 | 通话与录音降噪、语音增强 | Na | Planned | P3 |
| SG-MEDIA-09 | 自定义表情资源包 | Na | Planned | P3 |
| SG-MEDIA-10 | 音乐封面服务与已保存音乐 | Ad Ni | Planned | P3 |

## TRANS 翻译、转写与 AI

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-TRANS-01 | 服务实例：OpenAI 兼容与 DeepL 协议、连接测试、密钥存入系统凭据库（H03） | Ni | Implemented | P1 |
| SG-TRANS-02 | 翻译服务选择与草稿翻译（H01） | Ad Ni | Implemented | P1 |
| SG-TRANS-03 | 更多服务：Google、Yandex、Microsoft、Transmart、DeepLX、Anthropic 与 Gemini 协议 | Ad Ni Na | Planned | P1 |
| SG-TRANS-04 | 简繁转换改用 OpenCC 词组级转换，三平台可用（替代现有系统逐字转换） | Na | Planned | P1 |
| SG-TRANS-05 | 语音转写服务（H02） | Ni | Implemented | P2 |
| SG-TRANS-06 | 系统 AI 草稿预览（H04，macOS） | Ni | Implemented | P2 |
| SG-TRANS-07 | Instant View 与选中文本翻译 | Na | Planned | P2 |
| SG-TRANS-08 | 按对话自动翻译，话题、对话、账号、全局四级继承 | Ni | Planned | P3 |
| SG-TRANS-09 | LLM 上下文翻译与摘要 | Ni | Planned | P3 |

## NET 网络与代理

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-NET-01 | 代理备注、测速、排序、清理失效节点、导入导出（在上游代理管理基础上补齐） | Na | Planned | P2 |
| SG-NET-02 | 代理自动切换与 VPN 感知 | Na | Planned | P3 |
| SG-NET-03 | 代理订阅（SIP008、Clash 等），新协议只通过外部代理程序接入 | Na | Planned | P3 |
| SG-NET-04 | 自定义 DoH 与 IP 策略 | Na | Planned | P3 |
| SG-NET-05 | 上传、下载性能档位（先做基准测试） | Na | Planned | P3 |

## ACCT 账号

| ID | 功能 | 来源 | 状态 | 优先级 |
| --- | --- | --- | --- | --- |
| SG-ACCT-01 | 放开本地登录账号数量上限 | Na | Planned | P2 |
| SG-ACCT-02 | 账号作用域设置的双账号隔离验收 | D | Planned | P1 |

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
| SG-TG-02 | 每个上游版本（beta 与 stable）同步一次，同步后三平台 CI 通过才发布 | T | Planned | P0 |
| SG-TG-03 | 上游侵入预算：上游文件数、新增行数、挂钩数在 CI 中统计并设上限 | D | Planned | P0 |

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
