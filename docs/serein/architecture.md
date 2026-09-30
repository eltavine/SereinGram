# SereinGram 架构与重构方案

本方案参考 `pingora-panel` 的 ports-and-adapters 结构：上游框架（Telegram Desktop）只出现在适配器、界面层和挂钩门面中；核心模型、用例与存储契约不依赖上游。取舍记录见 [ADR](adr/)。

## 1. 现状基线（2026-09-30）

| 项目 | 数值 | 来源 |
| --- | --- | --- |
| 自有代码 | `Telegram/SourceFiles/nagram/` 144 个文件、13,823 行，最大文件 650 行 | `wc -l` |
| 上游侵入（全部） | 192 个上游文件，+1,781／−539 行 | `git diff 0b4a7faa9d..HEAD`，排除自有目录与文档 |
| 上游侵入（源码） | 123 个文件，+1,386／−339 行；最多的是 `history_widget.cpp` +87 行 | 同上，限 `Telegram/SourceFiles` |
| 设置声明 | 手写 `Option<T>` 注册表 + 24 个手写设置页文件（3,017 行） | `nagram/core`、`nagram/settings` |
| 结构化配置 | 过滤、链接、服务、菜单各自手写 JSON 解析与校验 | `nagram/*/model.cpp` |
| 构建 | 只在开发者本机做过 macOS Debug 构建；三平台 CI 从未运行 | `docs/nagram/design.md` |

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
| GHOST 已读 | `data/data_histories.cpp`、`data/data_replies_list.cpp`、`apiwrap.cpp`、`menu/menu_send.cpp` | `AllowReadReceipt` |
| GHOST 在线 | `api/api_updates.cpp` | `AllowOnlineStatus` |
| GHOST 活动状态 | `api/api_send_progress.cpp`、`history/history_streamed_drafts.cpp`、`chat_helpers/emoji_interactions.cpp` | `AllowSendAction` |
| GHOST 动态 | `data/data_stories.cpp` | `AllowStoryRead` |
| GHOST 浏览数 | `api/api_views.cpp` | `AllowViewIncrement` |
| HIST 删除 | `data/data_session.cpp` 的 `processMessagesDeleted`、`processNonChannelMessagesDeleted` | `OnMessagesDeleted` |
| HIST 编辑 | `history/history_item.cpp` 的 `applyEdition` | `OnBeforeEdition` |

已读类请求被拦截时，上游本地状态仍需按“已读”推进，否则未读计数与重试逻辑会卡住；这一点在 GHOST 模块的实现与测试中单独验证。

现有 Nagram 内联挂钩（123 个上游源文件）按功能族改走门面。预算：迁移完成后上游源码文件 ≤ 90 个、新增行 ≤ 900 行（不含品牌与构建文件），由 `tools/serein/upstream_budget.py` 与上游合并基线比较并在 CI 报告。

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
- 已迁移：界面、会话列表、消息、输入、媒体、隐私 6 个页面共 103 个选项；原 `options.h` 只转发到生成头文件。其余 8 个结构化 JSON 选项（菜单、服务、过滤、链接、别名、截图等）改用生成的编解码后再迁移。
- 编解码：带文件选项的 proto 生成 `serein/schema/gen/<目录>/<名>.h/.cpp`（值类型、`Read`/`Write`/`Validate`、文档级 `Parse…`/`Serialize…`），手写运行时只有 `serein/schema/codec.h`。生成的 `.cpp` 列在 `serein/schema/gen/sources.cmake`，主构建与测试构建都直接引入。第一个使用者是消息历史记录 `proto/serein/history/v1/record.proto`。
- 生成代码提交入库：三平台与发行版构建不需要 Buf 或 Python 依赖；CI 运行 `buf lint`、`tools/serein/proto_breaking.sh` 与 `generate.py --check`。

## 6. 存储（ADR-0003）

| 数据 | 位置 | 格式 |
| --- | --- | --- |
| 设备设置 | 上游 `Core::Settings` 的偏好 KV，每个页面一个键 `serein.<页面>` | schema 的 JSON |
| 账号设置 | 上游 `Storage::Account` 的偏好 KV | schema 的 JSON |
| 凭据 | 系统凭据库（QtKeychain，ADR-0004） | 不进偏好、不导出 |
| 消息历史 | 账号数据目录下 `serein/history.sqlite3`，Qt SQL + SQLite | 元数据列 + 加密载荷（`HistoryRecord` 的 JSON，含原始 TL 与 layer） |

历史库用 `PRAGMA user_version` 管理迁移；每个格式版本在 `serein/tests/fixtures/history/vN/` 保留不可变样本，测试必须能读取所有受支持版本并拒绝未知版本。

## 7. 守卫

| 守卫 | 工具 | 状态 |
| --- | --- | --- |
| 自有源文件 ≤ 1000 行 | `tools/serein/check_file_size.py` + `serein-guards.yml` | 已实施 |
| 模块依赖方向 | `tools/serein/check_boundaries.py` 按 `policy/boundaries.json` 检查每个 `#include`：`schema`、`ports`、`adapters` 严格执行；旧功能目录暂归宽松的 `serein/` 兜底规则，迁移一个收紧一个 | 已实施 |
| 上游侵入预算 | `tools/serein/upstream_budget.py`，与 `policy/upstream.json` 记录的上游基线比较；预算只降不升，并统计上游文件直接包含非门面头文件的数量 | 已实施 |
| schema 兼容 | `buf lint`；`tools/serein/proto_breaking.sh` 与推送前的提交或 PR 目标分支比较（`FILE` 级） | 已实施 |
| 生成代码漂移 | `uv run tools/serein/codegen/generate.py --check` | 已实施 |
| 三语文案一致 | `test_serein` | 已有 |
| 核心逻辑测试（只依赖 Qt） | `tools/serein/core_tests` 独立 CMake 工程，与主构建共用 `Telegram/cmake/serein_tests.cmake` 的测试清单 | 已实施 |
| 构建与单元测试 | `serein-{mac,win,linux}.yml` | 已有 |

本机运行核心逻辑测试（macOS 用 Homebrew 的 qtbase）：

```bash
cmake -S tools/serein/core_tests -B out/serein-core-tests -G Ninja \
    -DCMAKE_PREFIX_PATH="$(brew --prefix qtbase)"
cmake --build out/serein-core-tests && ctest --test-dir out/serein-core-tests
```

## 8. 迁移步骤

Phase 0 基础（全部 P0，每步独立提交、可回退）：

1. 文档与守卫：本目录；源文件行数守卫接入 CI；边界与预算守卫先以报告模式运行。
2. 更名：`nagram` → `serein`（目录、命名空间 `Nagram` → `Serein`、文案键 `lng_nagram_` → `lng_serein_`、存储键 `nagram.` → `serein.`、CMake、测试目标、工作流）。不迁移旧数据。
3. 品牌：应用名、图标、应用 ID、数据目录、链接（见第 9 节待定项）。
4. Schema：`proto/`、Buf、生成器；先让一个功能族（消息）端到端跑通，再迁移其余；随后删除手写注册表与通用设置页代码。
5. 挂钩门面：现有内联挂钩改走 `serein/hooks`，达到第 4 节预算。
6. 依赖替换：OpenCC、QtKeychain、测试框架（ADR-0004）。
7. 三平台 CI 通过。

Phase 1（P1）：GHOST、HIST、SG-FILTER-03 之前的过滤项补验、SG-PRIV-02、SG-TRANS-03、SG-TRANS-04、SG-ACCT-02。

Phase 2 与 Phase 3：按功能矩阵的 P2、P3；P3 每项先写 ADR 再实施。

## 9. 维护者决定（2026-09-30）

| 事项 | 决定 |
| --- | --- |
| 应用 ID | `io.github.eltavine.SereinGram` |
| 图标 | 先使用自有的临时占位图标，之后替换为正式图标；不沿用 Nagram 或 Telegram 图标 |
| 推送与 CI | 允许推送到 `main` 触发三平台工作流 |
| 服务条款风险功能 | SG-HIST-09、SG-PRIV-07、SG-PRIV-08 正常纳入 |
| proto3 方案 | 按 ADR-0002：proto3 + Buf + 自有生成器，不引入 protobuf 运行时 |
| 本机工具链 | 允许用 Homebrew 安装 qtbase，用于本机编译不依赖上游的核心逻辑测试 |

仍待提供：发布用 API 凭据（维护者在 my.telegram.org 申请，放入仓库 Secrets；未配置时 CI 使用上游公开测试凭据）。

本机没有 Xcode 与 `../Libraries`，暂时无法本地构建；在准备好本地工具链（`docs/building-mac.md`）之前，编译验证依赖 CI。
