# 分步实施计划

本文件把 [设置页设计](settings-page.md) 的条目拆成可独立提交的步骤。每个步骤的上游改动见 [上游处理点](upstream-hooks.md)，架构约束见 [设计与路线](design.md)。

## 1. 旧代码的处理

旧实现（`main` 分支）不要求复用。按模块给出建议，实现时逐文件审查后决定：

| 旧模块 | 建议 | 理由 |
| --- | --- | --- |
| `nagram/nagram_settings.*` | 不用 | 被选项注册表取代 |
| `settings/sections/settings_nagram.cpp`、`settings/settings_nagram_*.cpp` | 不用 | 按新页面设计重写 |
| `nagram/nagram_text.*`、`nagram/nagram_chinese.cpp`（间距、Markdown、简繁） | 参考算法，补单元测试后移植 | 纯逻辑，旧版有实体偏移处理经验 |
| `nagram/nagram_filters.*`、`nagram/nagram_links.*` | 参考解析与匹配逻辑；存储改为 `Storage::Account` | 纯逻辑可复用，存储方式必须改 |
| `nagram/nagram_services.*`、`nagram_service_request.*`、`nagram_credentials.*`、`nagram_translation.*` | 审查安全性（地址校验、凭据绑定）后移植 | 功能完整，但涉及密钥与网络请求 |
| `nagram/nagram_system_ai.*`（含 Swift） | 移植；CMake 中的 Swift 配置移入 `nagram.cmake` | 平台能力代码，改动小 |
| `nagram/nagram_config.*` | 参考校验规则，改为从注册表生成 | 旧版按手写列表校验 |
| `nagram/nagram_menu.*`、`nagram_repeat.*`、`nagram_batch.*` | 重写；复读等动作的发送逻辑可参考 | 菜单模型整体更换 |
| `nagram/nagram_snapshot.*`、`nagram_stickers.*`、`nagram_chat_sort.*`、`nagram_folders.*`、`nagram_main_menu.*`、`nagram_profile.*`、`nagram_media.*`、`nagram_sending.*`、`nagram_reading.*` | 参考；上游挂钩部分重写 | 挂钩方式与存储方式变化 |
| 旧文案（`lang.strings` 中的 `lng_nagram_*`、`nagram_zh-hans/hant.strings`） | 标题不变的条目直接沿用三语译文 | 已有人工校对的译文 |

旧版数据（本机偏好、账号数据、配置文件、凭据）不迁移。

## 2. 提交规则

1. **按小分组提交**：一个提交对应 [设置页设计](settings-page.md) 中的一个小分组（例如“消息 → 标记与计数”）。分组内条目依赖不同里程碑的基础设施时，拆到对应里程碑，在下表中注明。
2. **提交内容完整**：每个功能提交同时包含注册表条目、设置页行、三语文案、上游挂钩、单元测试（纯逻辑部分）。不存在“先加开关、后接逻辑”的提交。
3. **提交信息**：`feat(<分栏>): <分组名>`，正文列出条目编号、上游改动文件和验证方式。例：`feat(messages): 标记与计数`，正文 `C05–C09；history_view_bottom_info.cpp、data/data_session.cpp；test_nagram + 普通聊天与话题手动检查`。
4. **共用机制随第一个使用者提交**：消息视图刷新随 S20，会话列表刷新随 S30，输入区按钮可见性随 S34，重启提示随 S50；不单独提交没有使用者的代码。
5. **后续修复并入原提交**：同一分组的修复与评审修改用 `jj squash` / `jj absorb` 并入该分组的提交，不追加修补提交。
6. **命名**：稳定存储键为 `nagram.<lowerCamelCase>`；条目与旧实现 `Nagram::Option` 语义相同时沿用旧键名（旧键见 `main` 分支 `Telegram/SourceFiles/nagram/nagram_settings.h` 的 `OptionDefinition` 表，可用 `jj file show -r main <路径>` 查看）。文案键为 `lng_nagram_<snake_case>`，说明文字加 `_about` 后缀。C++ 代码放在 `namespace Nagram`，风格遵循仓库 `AGENTS.md`。

## 3. 编译验证点

| 级别 | 时机 | 内容 |
| --- | --- | --- |
| V1 每个提交 | 提交前 | 本地 macOS Debug 增量构建；运行 `test_nagram`；按提交正文列出的场景手动检查 |
| V2 里程碑 | 每个里程碑最后一个提交后 | rebase 到最新上游 `dev`；本地完整构建；三平台 CI（`nagram-mac/win/linux.yml`）通过；隔离数据目录启动冒烟（登录页、设置页、打开一个聊天）；在 `design.md` 更新里程碑状态 |
| V0 首次 | S10 | 第一次完整构建，同时验证已提交但尚未编译的文案管线（S01）、品牌（S02）与构建配置（S04）；发现的问题并入对应提交 |

构建目录使用仓库外或 `out/` 下的独立目录；API 凭据来自环境变量 `NAGRAM_API_ID` / `NAGRAM_API_HASH` 或 `Telegram/build/api_credentials.local.cmake`（见该目录下的 `.example`），本地调试也可用 `-D TDESKTOP_API_TEST=ON`。

## 4. 步骤

状态：✅ 已提交，☐ 待做。编号是本计划内的标识，不写进提交信息。

### M0 基础设施

| 步骤 | 提交 | 内容 | 验证 |
| --- | --- | --- | --- |
| ✅ S00 | `docs: Nagram 需求整理与重新设计路线` | 需求、来源目录、设计与路线 | — |
| ✅ S01 | `build: 独立的 Nagram 文案文件` | `langs/nagram/nagram.strings` 在配置阶段与上游 `lang.strings` 合并 | 合并脚本单独运行，输出与直接拼接一致；V0 macOS 编译通过 |
| ✅ S02 | `feat: Nagram 品牌与应用标识` | 应用名、应用 ID、图标、打包配置、关于页与托盘文案；关闭上游自动更新和崩溃上报 | 静态核对文案键与资源；V0 macOS 编译通过 |
| ✅ S03 | `docs: Nagram 设置页、上游处理点与实施计划` | 设置页设计、上游处理点、本计划 | — |
| ✅ S04 | `build: Nagram 构建配置与 API 凭据来源` | `Telegram/cmake/nagram_api.cmake`；本地凭据文件模板与忽略规则；`nagram-mac/win/linux.yml` 工作流 | 凭据解析 7 种场景用 CMake 单独验证；工作流通过 YAML 解析与 actionlint；V0 macOS 编译通过，首次 CI 待验证 |
| ✅ S10 | `build: Nagram 源文件清单与单元测试目标` | `Telegram/cmake/nagram.cmake`（`Telegram/CMakeLists.txt` 一行引入）；`test_nagram` 目标，先只含文案一致性测试；三个工作流加入 `test_nagram` 构建与运行 | V0：macOS arm64 Debug 完整构建与 `test_nagram` 通过 |
| ✅ S11 | `feat(core): 选项注册表与本机存储` | `Option<T>` 句柄、`Get/Value/Set`、校验、读取失败保留原值、变更通知；`Core::Settings` 偏好 | V1；单元测试覆盖默认值、往返、校验、通知去重 |
| ✅ S12 | `feat(core): 账号作用域存储` | `Storage::Account` 偏好读写；账号切换与退出的生命周期 | V1；双账号隔离单元测试；真实双账号场景待验证 |
| ✅ S13 | `feat(lang): 简繁内置文案` | `langs/nagram/zh-hans.strings`、`zh-hant.strings`；`lang_instance.cpp` 一处挂钩；含品牌文案译文 | V1；三语键与占位符一致性测试；缺少任一简繁文件时 `test_nagram` 必须失败 |
| ✅ S14 | `feat(settings): Nagram 设置入口与首页` | 设置主页顶部入口；首页头部；分栏在有条目后显示；搜索注册 | V1 编译与登录页冒烟通过；账号内设置页和 125%/200% 缩放待验证；**V2（M0 结束）** |

### M1 消息（分栏 3）

| 步骤 | 提交 | 条目 | 备注 |
| --- | --- | --- | --- |
| ✅ S20 | `feat(messages): 时间与信息` | C01–C04 | 消息视图刷新已接入；V1 macOS 编译和纯逻辑测试通过，账号内场景待 M1 V2 验证 |
| ✅ S21 | `feat(messages): 标记与计数` | C05–C09 | V1 macOS 编译与 `test_nagram` 通过；账号内场景待 M1 V2 验证 |
| ✅ S22 | `feat(messages): 反应` | C10–C15 | V1 macOS 编译与 `test_nagram` 通过；账号内场景待 M1 V2 验证 |
| ✅ S23 | `feat(messages): 特效` | C16–C18 | V1 macOS 编译与 `test_nagram` 通过；账号内场景待 M1 V2 验证 |
| ☐ S24 | `feat(messages): 内容显示` | C19–C24 | C25、C26 在 M5（依赖文本投影）；**V2** |

### M2 列表、输入、媒体、资料（分栏 2、4、6、7）

| 步骤 | 提交 | 条目 | 备注 |
| --- | --- | --- | --- |
| ☐ S30 | `feat(chats): 列表布局` | B01–B04 | 随附会话列表刷新机制 |
| ☐ S31 | `feat(chats): 文件夹` | B06–B08 | B05 在 M4（账号作用域，依赖文件夹选择界面） |
| ☐ S32 | `feat(chats): 推广内容` | B10–B13 | |
| ☐ S33 | `feat(chats): 滚动导航` | B14、B15 | |
| ☐ S34 | `feat(compose): 输入框按钮` | D01–D11 | 随附输入区按钮可见性机制 |
| ☐ S35 | `feat(compose): 输入行为` | D12–D15 | |
| ☐ S36 | `feat(compose): 发送确认` | D22–D26 | |
| ☐ S37 | `feat(compose): 转发` | D27 | |
| ☐ S38 | `feat(media): 贴纸与表情` | F01–F08 | |
| ☐ S39 | `feat(media): 播放` | F09 | F10 在 M7 |
| ☐ S3A | `feat(privacy): 本机隐私` | G01、G03、G04 | G02（演示模式）在 M7 |
| ☐ S3B | `feat(privacy): 资料信息` | G05–G08 | **V2** |

### M3 消息菜单（分栏 5）

| 步骤 | 提交 | 条目 | 备注 |
| --- | --- | --- | --- |
| ☐ S40 | `feat(menu): 菜单显隐与排序` | E01–E14 | 先按设计文档第 3.4 节做原型并记录结论；包含上游菜单项标记、`Apply`、菜单设置页 |
| ☐ S41 | `feat(menu): 复读与无引用转发` | E15–E17、E24 | 新增项默认隐藏 |
| ☐ S42 | `feat(menu): 批量与选择` | E18、E19 | 预览后执行，不自动发送 |
| ☐ S43 | `feat(menu): 媒体信息` | E20 | |
| ☐ S44 | `feat(core): 结构化配置与导出核心` | — | 以菜单配置为第一个结构化对象；版本化 JSON 校验；不含界面；**V2** |

### M4 界面与导航（分栏 1、2 其余）

| 步骤 | 提交 | 条目 | 备注 |
| --- | --- | --- | --- |
| ☐ S50 | `feat(interface): 圆角与形状` | A02–A04 | 随附重启提示机制；A01 已取消 |
| ☐ S51 | `feat(interface): 消息样式` | A05–A10 | |
| ☐ S52 | `feat(interface): 主菜单` | A11 | |
| ☐ S53 | `feat(interface): 窗口与通知` | A12–A14 | |
| ☐ S54 | `feat(interface): 界面文本` | A15 | |
| ☐ S55 | `feat(chats): 启动时打开的文件夹` | B05 | 第一个账号作用域条目 |
| ☐ S56 | `feat(chats): 会话排序` | B09 | |
| ☐ S57 | `feat(chats): 仅显示我管理的群组和频道` | 文件夹菜单 | **V2** |

### M5 文本与服务（分栏 3、4 其余，分栏 8）

| 步骤 | 提交 | 条目 | 备注 |
| --- | --- | --- | --- |
| ☐ S60 | `feat(compose): 文本格式` | D16–D21 | 间距算法与实体偏移测试随本提交 |
| ☐ S61 | `feat(messages): 阅读显示转换` | C25、C26、E22 | 随附显示文本投影机制 |
| ☐ S62 | `feat(ai): 服务实例与系统凭据` | H03 | |
| ☐ S63 | `feat(ai): 翻译` | H01、草稿翻译 | 含系统翻译 |
| ☐ S64 | `feat(ai): 语音转写` | H02 | |
| ☐ S65 | `feat(ai): 系统 AI` | H04 | **V2** |

### M6 规则与截图（分栏 9、菜单）

| 步骤 | 提交 | 条目 | 备注 |
| --- | --- | --- | --- |
| ☐ S70 | `feat(rules): 消息过滤` | I01、E23 | 账号作用域；正则预算与性能测试随本提交 |
| ☐ S71 | `feat(rules): 链接规则` | I02 | |
| ☐ S72 | `feat(menu): 消息截图` | E21 | **V2** |

### M7 其余条目与配置管理

| 步骤 | 提交 | 条目 | 备注 |
| --- | --- | --- | --- |
| ☐ S80 | `feat(privacy): 演示模式` | G02 | |
| ☐ S81 | `feat(privacy): 本地别名` | 资料页菜单 | 账号作用域 |
| ☐ S82 | `feat(media): 播放控制与文件发送` | F10、F11 | |
| ☐ S83 | `feat(media): 贴纸目录` | F12、F13 | |
| ☐ S84 | `feat(config): 配置管理` | J01–J04 | **V2** |

### P3

P3 功能不在本计划内。每项先在 `docs/nagram/` 下单独写设计并确认，再按本文件的规则拆分步骤。

## 5. 每个功能提交的检查清单

1. 注册表条目：键名、类型、默认值、作用域、分栏、是否可导出、是否需重启。
2. 设置页行：位置与 [设置页设计](settings-page.md) 一致；说明文字只在设计要求时出现。
3. 三语文案：英文、简体、繁体同时提交，`test_nagram` 的一致性检查通过。
4. 上游改动：只包含 [上游处理点](upstream-hooks.md) 中列出的位置；超出时在提交正文说明原因并更新该文档。
5. 两套消息视图（普通聊天、话题/计划消息）都生效（显示类条目）。
6. 关闭开关后行为与上游完全一致。
7. 纯逻辑部分有单元测试；界面部分记录手动检查的场景。
8. 通过对应级别的编译验证点（第 3 节）。
