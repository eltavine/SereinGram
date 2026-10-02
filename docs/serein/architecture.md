# SereinGram 架构与重构方案

本方案参考 `pingora-panel` 的 ports-and-adapters 结构：上游框架（Telegram Desktop）只出现在适配器、界面层和挂钩门面中；核心模型、用例与存储契约不依赖上游。取舍记录见 [ADR](adr/)。

## 1. 现状

| 项目 | 现状与数据来源 |
| --- | --- |
| 自有代码 | `Telegram/SourceFiles/serein/`；手写源文件不超过 1000 行（`tools/serein/check_file_size.py`），生成代码在 `schema/gen`、`settings/gen`、`hooks/gen` |
| 上游侵入 | 当前数值与上限由 `tools/serein/upstream_budget.py` 输出，上限记录在 `tools/serein/policy/upstream.json`；直接包含内部头文件的上游文件锁定为 0 |
| 设置页 | 布局、开关、数值、单选与文本行由 proto 生成；手写设置页只保留自定义控件 |
| 结构化配置 | 链接、快捷回复、过滤、主菜单、消息菜单、服务与历史记录均由 proto3 声明，生成编解码器校验 |
| 构建 | 三平台工作流在 PR 中构建 Debug 应用并运行 `test_serein` 与启动冒烟测试；`serein-release.yml` 每天调用同一批工作流构建 Release 矩阵（含 Windows arm64 与 Flatpak arm64）并发布 Nightly；Arch 与 Flatpak 工作流在各自文件改动与每周定时时运行；核心测试与守卫在每次推送时运行 |

上游刻意不带 protobuf 运行时：cld3 用手写头文件替代生成代码（`cmake/external/cld3`），WebRTC 以 `WEBRTC_ENABLE_PROTOBUF=0` 构建。静态 Qt 只初始化 `qtbase`、`qtimageformats`、`qtshadertools`、`qtsvg`，但没有关闭 Qt SQL，Qt 自带的 SQLite 驱动可用。

## 2. 目标结构

```text
proto/                                   proto3 schema 与 Buf 配置
tools/serein/                            守卫脚本、代码生成器（Python，uv 锁定依赖）
Telegram/cmake/serein.cmake              自有源文件清单与测试目标（CMakeLists.txt 一行引入）
Telegram/Resources/langs/serein/         三语文案
Telegram/SourceFiles/serein/
  schema/gen/     生成代码（提交入库，CI 校验无漂移，禁止手改）
  core/           选项读写、作用域解析、变更通知、校验、模块注册接口
  ports/          抽象端口：设备存储、账号存储、历史库、凭据、HTTP、翻译、转写、文本转换、时钟
  adapters/       端口实现：tdesktop、qtsql、keychain、opencc、platform/{mac,win,linux}
  hooks/          上游唯一允许包含的 serein 头文件；默认返回上游行为
  features/<名>/  model/ 纯逻辑与状态机；ui/ 界面、菜单项、设置子页；module.cpp 注册
  settings/       由 schema 元数据生成设置页与搜索索引
  app/            组合根：创建适配器、注册模块、把 hooks 连到模块
  tests/          单元测试、假实现、界面场景
```

## 3. 模块职责与依赖方向

箭头表示左侧依赖右侧：

```text
features/*/model -> core + ports + schema
features/*/ui    -> features/*/model + lib_ui + 上游界面 API
features/*/module.cpp -> 本功能 model、ui + hooks 注册接口 + settings 注册接口
settings  -> core + schema + lib_ui + 上游 Settings API
adapters  -> ports + 上游 + 第三方库
hooks     -> core（不依赖任何功能模块）
app       -> 全部（只做组装）
上游文件  -> hooks（仅此一处）
```

| 模块 | 职责 | 禁止知道 |
| --- | --- | --- |
| `schema` | 值类型、字段元数据、JSON 编解码、校验 | 上游、功能模块 |
| `core` | 选项 API、作用域、变更通知、模块注册接口 | 上游 Telegram 类型、Qt Widgets |
| `ports` | 抽象接口与值对象 | 任何实现 |
| `adapters` | 把端口接到上游和第三方库 | 功能规则 |
| `hooks` | 上游调用点的稳定签名与分发 | 功能模块 |
| `features/*/model` | 功能规则，可单元测试 | 上游、Qt Widgets、其他功能 |
| `features/*/ui` | 该功能的界面 | 其他功能 |
| `settings` | 通用设置页生成 | 功能规则 |
| `app` | 组合根 | 业务规则 |

扩展规则：

1. 上游文件只允许包含 `serein/hooks/*.h`；每个挂钩在上游只占一行调用或一个条件，逻辑放在 `serein/`。品牌与构建文件是唯一例外，集中在品牌提交中。
2. 优先使用上游已有扩展点：`rpl` 事件（`Main::Domain`、`Main::Account::sessionChanges()`、`Data::Session` 的各类变更流）、样式常量、`Settings::Section` 注册。只有这些都做不到时才新增挂钩。
3. 功能模块之间不直接包含；共享能力放进端口或 `core` 的事件。
4. 新功能 = 新目录 `features/<名>` + schema 字段 + `module.cpp` 注册，不修改核心流程。
5. 生成的 schema 类型是值类型；持久化格式属于适配器。
6. schema 只做增量演进，删除字段改为 `reserved`；`buf breaking` 强制执行。
7. 挂钩的默认实现等于上游行为；没有模块注册处理器时，客户端行为与上游一致。

## 4. 挂钩门面

```cpp
// serein/hooks/ghost.h（上游只包含这个头文件）
namespace Serein::Hooks {

[[nodiscard]] bool AllowReadReceipt(not_null<PeerData*> peer);
[[nodiscard]] bool AllowOnlineStatus(not_null<Main::Session*> session);

} // namespace Serein::Hooks
```

`hooks/*.cpp` 持有处理器表，功能模块在 `module.cpp` 中安装处理器，门面据此分发；没有处理器时返回上游默认值。上游调用点示例：`if (!Serein::Hooks::AllowReadReceipt(peer)) { ... }`。

新功能的计划挂钩点（已核对请求在上游的构造位置）：

| 功能 | 上游位置 | 门面函数 |
| --- | --- | --- |
| GHOST 已读 | `data/data_histories.cpp`（对话）、`data/data_replies_list.cpp`（话题与评论）、`data/data_saved_sublist.cpp`（频道私信）、`apiwrap.cpp`（内容已读） | `AllowReadReceipt`、`ReadInboxLocally` |
| GHOST 在线 | `api/api_updates.cpp` | `AllowOnlineStatus` |
| GHOST 活动状态 | `api/api_send_progress.cpp`、`history/history_streamed_drafts.cpp`、`chat_helpers/emoji_interactions.cpp` | `AllowSendAction` |
| GHOST 动态 | `data/data_stories.cpp` | `AllowStoryRead` |
| GHOST 浏览数 | `api/api_views.cpp` | `AllowViewIncrement` |
| HIST 删除 | `data/data_session.cpp` 的 `processMessagesDeleted`、`processNonChannelMessagesDeleted` | `OnMessagesDeleted` |
| HIST 编辑 | `history/history_item.cpp` 的 `applyEdition` | `OnBeforeEdition` |

已接入：在线状态（`api/api_updates.cpp`）与输入状态（`api/api_send_progress.cpp`，群通话的“正在说话”不受影响），各为一行条件；服务器删除（`data/data_session.cpp` 两处）与编辑前快照（`history/history_item.cpp`），实现位于组合根 `serein/app/`。已读类请求被拦截时，上游本地状态仍需按“已读”推进，否则未读计数与重试逻辑会卡住；这一点在 GHOST 模块的实现与测试中单独验证。

门面的三种来源：设置选项的取值与订阅函数由 proto 生成到 `serein/hooks/gen/<页>.h`；面向上游的薄接口头文件位于 `serein/hooks/<领域>/`，只允许前置声明与库头文件，可脱离应用代码单独通过语法检查（参数或返回值是上游嵌套类型时，门面声明为函数模板，由实现文件对该类型显式实例化，例如 `ApplyInfoOptions(Data &, ...)` 与 `TranscriptionOverride<Entry>(item)`；只需填充上游私有结构而不读取其他成员时，模板直接写在门面头文件里，由上游传入自身类型，例如 `ModerateDefaults<ModerateMessagesBoxOptions>()` 与 `PrependCustomDoh(attempts, Type::Mozilla)`）；其余一次性挂钩（幽灵、历史、定时发送）位于 `serein/hooks/*.h`，实现放在组合根 `serein/app/`。应用启动只有一个挂钩 `Serein::Hooks::OnApplicationStarted()`：组合根的模块表 `serein/app/modules.cpp` 为每个模块登记“应用启动”“会话启动”和“窗口启动”回调（窗口启动由 `SessionController` 构造函数中的 `Serein::Hooks::OnWindowStarted` 分发），会话跟踪统一订阅各账号的 `sessionValue()`，功能模块不再各自挂接上游。消息菜单的定制按菜单项文字识别上游动作，文字在每次打开菜单时按当前语言计算；只有文字与其他菜单项重复或由自绘控件显示的项（保存图片、带自动删除倒计时的删除、表情包按钮）在上游保留显式标签。

上游侵入由 `tools/serein/upstream_budget.py` 与上游合并基线比较，CI 报告当前数值；长期目标是上游源码文件不超过 90 个、新增行不超过 900 行（不含品牌与构建文件），途径是把品牌改动与多处小挂钩合并到门面。

中性默认值约定：Serein 向上游界面添加的任何入口（消息、对话、资料、主菜单、托盘、贴纸包、文件夹与输入框菜单中的项目，以及新的按钮与行）都必须由默认关闭的选项控制，选项关闭时上游界面保持原样；只有出现前提本身默认关闭的入口（例如依赖消息记录的“编辑历史”）可以不另设开关，并在测试中写明理由。只影响一次同步渲染的临时行为用作用域覆盖实现，例如消息截图在 `Snapshot::Render` 期间用 `Interface::ThemeReplyColorsScope` 让回复使用主题色，而不修改全局选项。通过消息列表委托安装的挂钩必须先排除 `Context::ChatPreview`，因为对话列表预览的委托把 `listWindow()` 实现为 `Unexpected()`。

## 5. Schema 与代码生成（ADR-0002）

```text
proto/buf.yaml, proto/buf.gen.yaml
proto/serein/options/v1/options.proto     自定义选项：标题文案键、页面、分组、控件、作用域、重启、可导出、平台
proto/serein/settings/v1/<族>.proto       每个功能族一个文件
proto/serein/config/v1/bundle.proto       配置导出包
proto/serein/history/v1/record.proto      历史记录载荷
```

```proto
message MessagesSettings {
  option (serein.options.v1.page) = { id: "messages" scope: SCOPE_DEVICE };
  optional bool show_seconds = 1 [(serein.options.v1.field) = {
    title: "lng_serein_show_seconds" section: "time" refresh: REFRESH_MESSAGE_VIEW }];
  optional int32 bubble_radius_percent = 2 [
    (serein.options.v1.field) = { title: "lng_serein_bubble_radius" control: CONTROL_PERCENT restart: true },
    (buf.validate.field).int32 = { gte: 10, lte: 100 }];
}
```

- 语义：proto3 `optional` 有值表示用户显式设置；无值表示跟随 Telegram。稳定标识是字段编号与 JSON 名，删除字段必须 `reserved`。
- 校验使用 protovalidate 的标准注解；生成器只接受其中的范围、枚举、长度约束，遇到不支持的约束直接报错。
- 生成器 `tools/serein/codegen`（`uv run tools/serein/codegen/generate.py`）读取 `buf build` 的 JSON 映像，用 Jinja2 为每个设置页生成 `serein/schema/gen/settings/<页>.h`：类型化的 `Option<T>` 句柄、校验与 `RegisterOptions`。命名约定：常量 `k<字段名驼峰>`、存储键 `serein.<json_name>`、标题 `lng_serein_<字段名>`，只有例外才写 `cpp_name`、`title`。
- 全部设置页的选项都由生成头文件声明；各功能目录的 `options.h` 只转发到生成头文件。
- 编解码：带文件选项的 proto 生成 `serein/schema/gen/<目录>/<名>.h/.cpp`（值类型、`Read`/`Write`/`Validate`、文档级 `Parse…`/`Serialize…`），手写运行时只有 `serein/schema/codec.h`。生成的 `.cpp` 列在 `serein/schema/gen/sources.cmake`，主构建与测试构建都直接引入。使用者包括消息历史记录 `proto/serein/history/v1/record.proto` 与 `proto/serein/config/v1/` 下的各结构化配置；`require_fields` 让编解码器拒绝缺少字段的文档，格式变更需要提升文档版本并提供迁移。
- 生成代码提交入库：三平台与发行版构建不需要 Buf 或 Python 依赖；CI 运行 `buf lint`、`tools/serein/proto_breaking.sh` 与 `generate.py --check`。

## 6. 存储（ADR-0003）

| 数据 | 位置 | 格式 |
| --- | --- | --- |
| 设备设置 | 上游 `Core::Settings` 的偏好 KV，每个页面一个键 `serein.<页面>` | schema 的 JSON |
| 账号设置 | 上游 `Storage::Account` 的偏好 KV | schema 的 JSON |
| 凭据 | macOS Keychain、Windows 凭据管理器；其他平台为用本地密钥加密的 `tdata/serein_credentials`（ADR-0004 修订） | 不进偏好、不导出 |
| 消息历史 | 账号数据目录下 `serein/history.sqlite3`，Qt SQL + SQLite | 元数据列 + 加密载荷（`HistoryRecord` 的 JSON，含原始 TL 与 layer） |

历史库用 `PRAGMA user_version` 管理迁移；每个格式版本在 `serein/tests/fixtures/history/vN/` 保留不可变样本，测试必须能读取所有受支持版本并拒绝未知版本。

## 7. 守卫

| 守卫 | 工具 | 状态 |
| --- | --- | --- |
| 自有源文件 ≤ 1000 行 | `tools/serein/check_file_size.py` + `serein-guards.yml` | 已实施 |
| 模块依赖方向 | `tools/serein/check_boundaries.py` 按 `policy/boundaries.json` 检查每个 `#include`：`schema`、`ports`、`adapters` 严格执行，其余目录适用宽松的 `serein/` 兜底规则 | 已实施 |
| 功能矩阵格式 | `tools/serein/check_features.py`：`features.md` 每行的 ID 唯一且形如 `SG-<族>-<两位序号>`，状态只能是 Planned、In Progress、Implemented、Verified，优先级 P0–P3，来源只用约定缩写 | 已实施 |
| 门面命名空间遮蔽 | `tools/serein/check_hook_namespaces.py`：生成的门面命名空间 `Serein::Hooks::<页>` 会遮蔽同名的 `Serein::<页>`；位于 `Serein::Hooks` 内、且包含了该门面的代码，只能用 `<页>::` 访问门面里声明的函数，其余名字必须写成 `Serein::<页>::` | 已实施 |
| 工作流静态检查 | actionlint 1.7.12（含 shellcheck）检查 `.github/workflows/serein-*.yml` 的表达式、矩阵属性、`needs` 引用、Action 输入与内嵌脚本；zizmor 1.16.3 检查工作流安全：第三方 Action 固定到提交哈希、检出不保留凭据、可复用工作流只接收需要的 Secrets、无模板注入；本地未安装 actionlint 时 `check_all.sh` 跳过并提示 | 已实施 |
| 格式与风格 | `tools/serein/check_style.py`：自有文本文件为无 BOM 的 UTF-8、只用 LF、以单个换行结尾、无行尾空白，YAML、Python、proto、JSON 与 CMake 不用制表符缩进，`tools/serein/policy/` 的 JSON 为规范格式；Serein C++ 用制表符缩进、不连续空行、`&&` 与 `\|\|` 置于续行开头、带访问区段的类在 `};` 前空一行、使用嵌套命名空间写法、类外定义不重复 `[[nodiscard]]`、注释按单行限额（多行须以 `// WHY:` 开头且不超过三行，测试目录除外），并禁用 `QStringLiteral`、`(void)` 与 `static_cast<void>`、`Q_OS_LINUX`、`NULL` 以及生产代码中的 `_DEBUG` 分支 | 已实施 |
| Python、Shell、YAML、文档与 proto 格式 | ruff 0.16.10 格式检查与 Lint（`tools/serein/ruff.toml`）；shellcheck；yamllint 1.37.1 严格模式（`tools/serein/yamllint.yml`）；markdownlint-cli2 0.19.1（`tools/serein/serein.markdownlint-cli2.jsonc`）；`buf format --diff --exit-code` | 已实施 |
| 静态分析 | clang-tidy 21.1.1 按 `Telegram/SourceFiles/serein/.clang-tidy` 检查核心测试工程中的全部手写 Serein 编译单元（bugprone、clang-analyzer、performance 等），由 `tools/serein/run_clang_tidy.py` 并行运行；只统计 Serein 自有位置的诊断，生成代码与上游头文件除外，无法解析的编译单元同样判为失败 | 已实施 |
| 密钥扫描 | gitleaks 8.30.1 扫描每次推送或 PR 新增的主线提交（`tools/serein/gitleaks.toml` 只放行打包说明中的占位凭据） | 已实施 |
| 桌面元数据 | `desktop-file-validate` 校验桌面入口，`appstreamcli validate` 校验 AppStream 元数据 | 已实施 |
| 启动冒烟测试 | `tools/serein/smoke_test.py`：三平台构建后以全新 `-workdir` 启动应用，要求日志出现启动行且进程在等待期后仍在运行；Linux 先用 `ldd` 确认运行库都能找到 | 已实施 |
| 发布结构 | `tools/serein/release.py verify`：发布前的产物集合必须与 `tools/serein/policy/release_assets.json` 完全一致，并生成 `SHA256SUMS` 与 `release.json` | 已实施 |
| 上游侵入预算 | `tools/serein/upstream_budget.py`，与 `policy/upstream.json` 记录的上游基线比较；预算默认只降不升；新功能需要新挂钩时，在同一提交中上调并在提交说明中写明增量与理由；同时统计上游文件直接包含非门面头文件的数量（已锁定为 0） | 已实施 |
| schema 兼容 | `buf lint`；`tools/serein/proto_breaking.sh` 与推送前的提交或 PR 目标分支比较（`FILE` 级） | 已实施 |
| 生成代码漂移 | `uv run tools/serein/codegen/generate.py --check` | 已实施 |
| 中性默认值 | `test_serein` 的 `TestNeutralDefaults`：全部设置页的选项默认关闭、为零或为空，消息菜单中 Serein 新增的项默认隐藏，例外逐项写明理由 | 已实施 |
| 头文件 | `tools/serein/check_includes.py`：Serein 代码中带引号的 `#include`，以及上游文件中引用 `serein/` 的 `#include`，必须指向仓库或已拉取子模块中存在的头文件，只在构建目录中生成的头文件（样式、语言键与 schema 生成物）跳过；Serein 代码不得包含平台目标缺少的系统头文件，目前为 `<filesystem>`（macOS 10.15 起才可用，改用 `QDir`、`QFileInfo`） | 已实施 |
| 源文件登记 | `tools/serein/check_sources.py`：每个 Serein 源文件都必须登记在 `Telegram/cmake/serein.cmake` 或测试清单中，反过来已登记的路径也必须存在 | 已实施 |
| 上游子模块指针 | `upstream_budget.py` 比较暂存区中与上游共有的子模块指针和上游基线，不一致即失败；有意保留的差异写入 `submodule_overrides` | 已实施 |
| 打包依赖版本 | `tools/serein/check_packaging.py`：Arch PKGBUILD 与 Flatpak 清单锁定的 tdlib、tg_owt、tlottie、patches 提交与 Qt 版本必须与 `snap/snapcraft.yaml` 一致；`--update` 自动改写提交 | 已实施 |
| 打包文件布局 | 工具自测运行 `packaging/nfpm/stage.sh`，检查 `.deb` 与 `.rpm` 需要的程序、桌面入口、元数据与各尺寸图标都能从上游路径取得 | 已实施 |
| 三语文案一致 | `test_serein` | 已有 |
| 核心逻辑测试（只依赖 Qt） | `tools/serein/core_tests` 独立 CMake 工程，与主构建共用 `Telegram/cmake/serein_tests.cmake` 的测试清单 | 已实施 |
| 构建与单元测试 | `serein-{mac,win,linux}.yml` | 已有 |

本机运行核心逻辑测试（macOS 用 Homebrew 的 qtbase）：

```bash
cmake -S tools/serein/core_tests -B out/serein-core-tests -G Ninja \
    -DCMAKE_PREFIX_PATH="$(brew --prefix qtbase)"
cmake --build out/serein-core-tests && ctest --test-dir out/serein-core-tests
```

## 8. 演进规则

1. 新功能先在功能矩阵登记 ID、来源与优先级；P3 或存在服务条款、平台能力风险的功能先写 ADR。
2. 设置项在 proto 中声明并重新生成代码，默认值必须让客户端行为与上游一致，`TestNeutralDefaults` 负责检查。
3. 逻辑放在 `features/` 或对应功能目录并配核心测试；上游只通过 `serein/hooks` 中的单行调用接入，需要新挂钩时在同一提交中按增量上调侵入预算。
4. 同步上游用 `tools/serein/upstream_sync.py`：合并后移动预算基线，并让打包配方的依赖版本跟随 snap 配方；合并后的三平台构建通过才算完成同步。

## 9. 项目约定

| 事项 | 约定 |
| --- | --- |
| 应用 ID | `io.github.eltavine.SereinGram` |
| 图标 | 自有的临时占位图标，之后替换为正式图标；不沿用 Nagram 或 Telegram 图标 |
| 功能范围 | 包含可能与服务条款冲突的功能（SG-HIST-09、SG-PRIV-07、SG-PRIV-08），与其他增强一样默认关闭 |
| proto3 方案 | 按 ADR-0002：proto3 + Buf + 自有生成器，不引入 protobuf 运行时 |
| 文案 | 英文文案 `langs/serein/serein.strings` 由 `Telegram/cmake/serein_lang.cmake` 并入上游的语言代码生成，界面代码照常使用 `tr::lng_serein_*`；生成的键查找函数再经 `tools/serein/split_lang_keys.py` 按键名首字母拆分后编译（MSVC arm64 拒绝编译单个过大的函数）；其他语言的译文按界面语言从资源中加载，缺失的键回退英文 |
| CI 缓存 | 整个仓库共用 10 GB 的 Actions 缓存，超出后按最久未访问淘汰；分支与 PR 只能读取本身和默认分支的缓存，所以只有 PR 构建与默认分支上的构建写入缓存，其他分支上的发布构建只读取：Windows 的依赖与 Qt 缓存在清理步骤之后保存（与上游一致），macOS 依赖缓存的键包含工具链指纹，Linux 缓存 Docker 层并只保留一份编译缓存；Windows 与 macOS 的依赖缓存同时包含 Debug 与 Release 版本，PR 与发布构建共用；发布流程中的 Windows arm64、Arch 与 Flatpak 构建不读写缓存，以免挤掉三个平台的缓存 |
| API 凭据 | 不使用官方 Telegram 客户端凭据；构建从仓库 Secrets 的 `SEREIN_API_ID` 与 `SEREIN_API_HASH` 注入，没有密钥的 fork 与 PR 构建回退到上游为开发构建公开提供的测试凭据（`TDESKTOP_API_TEST`） |
