# Nagram Desktop 设计与实施路线（nagram-next）

本文件取代旧实现分支（`main`）中 `docs/nagram-settings.md` 第 4–7 节的存储、接口与交付计划。需求见 [功能需求](requirements.md)，来源核对见 [功能与配置目录](feature-catalog.md)。

- 基线：上游 `telegramdesktop/tdesktop` 的 `dev`，本分支起点为 `Update HEIF decoding patches.`（`AppVersion = 7002009`，Qt 6.11.2）。
- 分支：`nagram-next`。旧分支 `main` 保留作参考，不再追加功能。
- 核心目标：功能按需求重新交付，但**上游改动面最小、可持续 rebase、每个功能包可独立审查与回退**。

## 1. 旧实现复盘

以下问题基于旧分支（上游 7.2.8 之上的 7 个提交）的实际 diff 核对，是本次重做的直接原因。

| 问题 | 证据 | 影响 | 新分支规则 |
| --- | --- | --- | --- |
| 账号数据追加到上游二进制流尾部 | `Main::SessionSettings::serialize()` 末尾追加启动文件夹、过滤、别名、管理文件夹 5 个字段；上游读取端对新增字段一律使用 `if (!stream.atEnd())` 顺序读取 | 上游以后在尾部加字段时，会把 Nagram 字节当成自己的字段读取，造成账号设置错乱；每次 rebase 都要人工对齐字段顺序 | 禁止向任何上游顺序序列化格式追加字段；账号数据使用上游已有的 `Storage::Account` 偏好 KV（`lskPrefs`，已加密、按账号） |
| 上游挂钩密度过高 | 85 个上游源文件引用 `nagram/` 头文件，662 处 `Nagram::` 调用；其中 `history_view_context_menu.cpp` 138 处、`history_inner_widget.cpp` 130 处、`history_widget.cpp` 56 处 | 上游已前进 207 个提交（含 7.2.9），这些热点文件是上游最常改动的文件，rebase 冲突成本高 | 挂钩只允许一行调用或一行标记，逻辑全部在 `nagram/`；每个里程碑统计并限制上游改动面（第 3.4 节） |
| 提交粒度失控 | `feat: implement localized Nagram desktop preferences` 一次改动 194 个文件、+14651 行，覆盖 P1 全部与 P2 大部分 | 无法按功能审查、二分定位或单独回退 | 每个功能包一个 change，文档、构建、品牌、CI 分开 |
| 文案直接写进上游资源 | `lang.strings` +539 行，`lang_instance.cpp` +81 行 | 上游每次改文案都可能冲突 | Nagram 文案放独立文件，构建时合并（第 3.5 节） |
| 同一模型反复迁移 | 消息菜单先有 9 个 `HideMenu*` 布尔键，再做 `nagram.messageMenu` v1，随后未发布前又加 v2 并写 v1 迁移 | 未发布格式背负迁移代码，菜单逻辑分散在两套上游菜单实现中 | 动作注册表先定稿再接入；未发布的格式不做迁移，直接定为 v1 |
| 仓库卫生 | `releases/v0.1.0-pre.1/Nagram.app/...` 的 plist、icns、rcc 被提交；两个空的 `ci: trigger rebuild` 提交；Qt 版本在两个提交间降级后又恢复；`dependabot.yml` 删除混在修复提交里；`Telegram/ThirdParty/MicroTeX` 子模块处于脏状态 | 仓库体积增长、历史难读、CI 行为不可追溯 | 发行物只放 GitHub Releases；CI 调整单独提交；Qt 版本只跟随上游 `qt_version.py` |
| 测试不留存 | 旧文档记录的上千项检查均为“临时测试场景”，交付前移除，证据在未提交的 `out/` 目录 | 回归无法复跑，rebase 后无从验证 | 纯逻辑测试留在仓库并可在 CI 运行；界面场景保留为可选的测试代理场景（第 3.7 节） |
| 需求、设计、进度混在一个文档 | `nagram-settings.md` 同时包含需求、稳定键、存储设计与各批验收日志 | 实现状态与需求互相污染，难以判断哪条是约束 | 拆为需求、设计与路线、来源目录三份；进度只记录在本文件第 5 节的里程碑状态 |

旧实现中经过验证的**行为结论**（例如边界条件、权限检查、上游能力复用清单）仍有参考价值，重做时按功能包逐个移植并重新审查，不整体 cherry-pick。

## 2. 设计原则

1. **上游优先复用**：上游已有的设置、存储、动作和页面直接使用，不在 Nagram 页重复入口（沿用需求第 2 节约定）。
2. **Nagram 逻辑内聚**：所有判断、解析、校验、网络请求与界面构建都在 `Telegram/SourceFiles/nagram/`；上游文件只保留调用点。
3. **数据格式不与上游交叉**：不修改上游二进制格式、不改上游 `lsk*` 键、不在上游结构体加持久化字段。
4. **一个声明来源**：每个选项只在注册表声明一次；设置页、搜索、配置交换、校验、默认值均从同一声明派生。
5. **默认不改变行为**：所有新选项默认关闭或继承；读取失败保留原载荷并报告，不写回默认值。
6. **可持续 rebase**：每个里程碑结束时必须能干净 rebase 到最新上游 `dev` 并通过构建与测试。

## 3. 架构

### 3.1 目录结构

```
Telegram/SourceFiles/nagram/
  core/         选项注册表、存储适配、响应式读取、作用域解析、JSON 校验工具
  settings/     Nagram 设置页（由注册表生成）、搜索注册、配置交换界面
  menu/         消息菜单动作注册表与菜单后处理
  display/      消息与列表显示类功能（F02/F03/F18）
  compose/      输入、格式、盘古间距、发送确认（F04/F05/F06 的发送部分）
  services/     凭据、翻译、转写、系统 AI（F07/F08）
  filters/      正则过滤、作者屏蔽、Zalgo（F09）
  links/        链接预览与 URL 规则（F16）
  snapshot/     消息截图（F13）
  tests/        纯逻辑单元测试
Telegram/Resources/langs/nagram/   Nagram 三语文案
```

子目录随功能落地再创建，不预先建空目录。构建清单单独放在 `Telegram/cmake/nagram.cmake`，由 `Telegram/CMakeLists.txt` 一行引入，避免在上游大文件列表中穿插 Nagram 源文件。

### 3.2 选项注册表

替代旧实现的大枚举加手写页面。每个选项是一个带类型的常量句柄，注册表是唯一声明来源：

```cpp
namespace Nagram {

enum class Scope { Device, Account };

template <typename T>
struct Option {
	std::string_view key;       // 稳定键，例如 "nagram.hideStories"
	Scope scope;
	T fallback;
	Category category;
	tr::phrase<> title;
	Flags flags;                // RequiresRestart / Exportable / Hidden ...
	bool (*validate)(const T &) = nullptr;
};

inline constexpr auto kHideStories = Option<bool>{
	"nagram.hideStories", Scope::Device, false, Category::Appearance,
	tr::lng_nagram_hide_stories, Flag::Exportable };

} // namespace Nagram
```

- 值类型限定为 `bool`、有范围的整数、单行字符串和带版本的 JSON 对象（`QByteArray`）。
- 读写接口：`Get(option)`、`Value(option)`（`rpl::producer`，订阅时先给当前值，之后去重推送）、`Set(option, value)`；账号作用域的读写额外带 `not_null<Main::Session*>`。
- 设置页、搜索索引、配置交换 allowlist、诊断报告都遍历注册表；未注册的键不会出现在任何界面或交换文件中。
- 未交付的功能不进入注册表（需求第 2 节“未实现功能不进入开关”）。

### 3.3 存储

| 作用域 | 存储位置 | 说明 |
| --- | --- | --- |
| D 本机 | 上游 `Core::Settings` 的 `readPref` / `writePref` / `clearPref` KV | 上游已提供，写入会触发上游延迟保存；写入默认值等于 `clearPref` |
| A 账号 | 上游 `Storage::Account` 的 `readPref` / `writePref` KV（`lskPrefs`） | 上游已提供、随账号加密与生命周期管理；替代旧实现追加 `SessionSettings` 尾部字段的做法 |
| C/T 对话/话题 | A 作用域下的版本化 JSON 映射，键为规范化 peer ID / topic root ID | 只为明确支持分层覆盖的选项建立；解析顺序 T → C → A → D → 默认 |
| 凭据 | macOS Keychain / Windows Credential Manager；其他平台明确报告不可用 | 不进入偏好、不导出；查找键绑定服务地址、协议和用途 |
| 大体量数据 | 独立的账号级加密文件（仅 P3 本地历史需要） | 进入实现前单独设计生命周期 |

响应式通知由 Nagram 自己维护的 `rpl::event_stream<std::string_view>` 提供，在 `Nagram::Set` 内部触发，不修改上游 `Core::Settings`。旧实现为原子导入新增的 `Core::Settings::applyPrefChanges()` 不再需要：导入先完成全部校验与冲突检查，再在同一事件循环内逐键写入，写入期间抑制通知，结束后统一推送一次。若 M0 验证发现上游逐键写入会产生可观察的中间状态，再以最小改动补一个批量接口。

结构化对象一律为 `{"version": N, ...}`，边界严格校验类型、枚举、范围和未知字段；解析失败保留原字节并在诊断中报告。

### 3.4 上游挂钩规则

1. 上游文件中的改动只允许三种形式：一行 `#include "nagram/..."`、一行对 `Nagram::` 的调用或条件、对已有控件的一行标记。需要多行逻辑时，把逻辑移入 `nagram/` 并暴露一个函数。
2. 优先使用上游已有扩展点：`rpl` 事件、`Data::Session` 通知、样式常量、`Window::SessionController` 生命周期、设置页的 `Settings::Section` 注册。
3. 两套消息列表实现（`HistoryInner` 与 `HistoryView::ListWidget`）只能通过同一个 `nagram/` 入口挂钩，不在两处重复写逻辑。
4. 每个里程碑结束时用 `jj diff --from dev --stat` 统计上游文件改动数和挂钩数，写入第 5 节；超出预算须在该里程碑说明理由。
5. 品牌相关改动（应用名、图标、平台标识）集中在一个 change 中，不与功能混合。

**消息菜单（F05）方案**：旧实现在两个菜单填充函数中插入了约 270 处调用。新方案分两步：

- 在上游创建菜单项的位置用一行 `Nagram::Menu::Tag(action, MenuAction::Reply)` 标记动作身份（不改变上游逻辑与顺序）。
- 上游填充完成后调用一次 `Nagram::Menu::Apply(menu, context)`，按注册表执行隐藏、重排、修饰键显示，并插入 Nagram 新增动作（复读、无引用转发、合并等）。权限与可用性仍由上游创建逻辑决定：未被上游创建的动作不会被 Nagram“恢复”。

M3 开始前先做一次原型，确认 `Ui::PopupMenu` 支持事后重排与隐藏；若不支持，退回为在填充函数入口传入一个过滤器，仍保持一行挂钩。

### 3.5 文案

- 英文放 `Telegram/Resources/langs/nagram/nagram.strings`，简繁放同目录 `zh-hans.strings`、`zh-hant.strings`，键统一以 `lng_nagram_` 开头。
- 构建时由 CMake 自定义命令把上游 `lang.strings` 与 `nagram.strings` 拼接到构建目录，再交给上游 `generate_lang`；上游 `lang.strings` 保持原样。`td_lang.cmake` 只改输入路径一行。
- 简繁内置文案作为缺失键的后备值，通过 `Lang::Instance` 的一个挂钩注入；不写入云端语言缓存，不覆盖语言包中显式提供的翻译。
- `test_nagram` 检查三语键集合一致、占位符一致、无重复键。

### 3.6 品牌

沿用旧实现已确定的品牌事实：应用名 Nagram、Linux 应用 ID `xyz.nextalone.nagram.desktop`、图标资源、Windows 安装脚本、snap 配置、关于页。作为 M0 中独立的 change 移植，并列出全部改动文件以便 rebase 时核对。

### 3.7 测试

- **`test_nagram`**：参照上游 `Telegram/cmake/tests.cmake` 新增可执行测试，只链接 `lib_base` 与 `nagram/core` 等纯逻辑代码。覆盖注册表默认值、类型校验、JSON 结构化对象、作用域解析、配置交换、正则过滤预算、盘古间距与实体偏移、URL 规则、文案一致性。在 `DESKTOP_APP_TEST_APPS` 下构建，CI 运行。
- **界面场景**：上游已有 `Test::` 测试代理（Debug 构建加 `-testagent`，使用标记为测试的一次性数据目录）。M0 评估能否让 Nagram 场景以独立文件注册、按环境变量选择，而不是像旧实现那样临时修改 `test_scenario.cpp` 后再删除。
- 每个功能包的验收沿用需求第 5 节；外部服务测试使用 localhost 桩，不向真实聊天发送消息。

### 3.8 版本控制与发布

- `nagram-next` 是建立在 `dev@upstream` 之上的一串 change：`docs` → `build/branding` → `core` → 各功能包。
- 同步上游：`jj git fetch --remote upstream` 后 `jj rebase -b nagram-next -d dev`，逐个 change 解决冲突并跑 `test_nagram`。
- 发行包只上传 GitHub Releases；`releases/` 等本地产物不进入仓库。
- CI 改动单独成 change；不提交空提交来触发构建。

## 4. 与旧版本的兼容

- **本机偏好**：语义不变的选项沿用旧稳定键（例如 `nagram.hideStories`），旧版 `v0.1.0-pre.1` 用户的 D 作用域设置可直接读取。语义改变的键换新名，不按键名猜测旧值。
- **账号数据**：旧版写入 `SessionSettings` 尾部的 5 个字段不迁移（预发布数据：启动文件夹、过滤规则、本地别名、管理文件夹）。为消除上游未来追加字段时误读旧字节的风险，新版本首次启动时对每个账号强制重写一次 `SessionSettings`（上游序列化不含这些字段），并用一个 D 作用域标记保证只执行一次。
- **配置交换文件**：新格式为版本 3；导入接受旧版本 2 中键名和语义未变的项，其余列为“未知项”跳过，不静默转换。
- **凭据**：旧版存入系统凭据库的服务密钥按新的查找键规则重新绑定；无法确认绑定关系时要求用户重新输入。

## 5. 实施路线

<a id="roadmap"></a>

优先级沿用需求中的 P1 / P2 / P3 分级（常用度 × 复杂度 × 依赖），按依赖关系重新编排为里程碑。每个里程碑的完成条件相同：macOS Debug 构建通过、`test_nagram` 通过、上游改动面统计已更新、能干净 rebase 到当时最新的上游 `dev`。

| 里程碑 | 内容 | 对应需求包 | 依赖 |
| --- | --- | --- | --- |
| M0 基础设施 | 品牌；`nagram.cmake`；文案管线；选项注册表；D/A 存储适配与响应式读取；设置页骨架与搜索；`test_nagram`；测试代理场景评估；首次启动重写 `SessionSettings` | P1-01、F17 基础 | 无 |
| M1 显示类开关 | 时间戳秒、转发原始日期、消息/资料 ID、DC、精确计数、隐藏动态/反应/赞助/推荐/会员装饰/频道底栏/快速分享、贴纸时间戳与尺寸、视频自动播放、服务消息时间、剧透 | P1-02、P1-03、P1-07 显示部分 | M0 |
| M2 列表与输入 | 紧凑列表、预览行数、文件夹计数与全部会话、归档入口；输入区按钮显隐、悬停弹出、命令填草稿；通话/贴纸/GIF/语音/圆视频确认、转发评论顺序、问候贴纸 | P1-04、P1-05 输入部分、P1-06 | M0 |
| M3 消息菜单 | 菜单原型验证；动作注册表与 `Tag` / `Apply`；显隐、排序、修饰键显示；复读、无引用复读/转发、合并、反向回复、快捷评价、同作者选择、批量收藏；配置交换核心（第一个结构化对象） | P1-05 菜单部分、P2-03 菜单部分、P2-04、P2-11 核心 | M0 |
| M4 外观与导航 | 等宽字体、气泡/头像圆角、消息宽度、尾部/引用/缩略图、忽略对话主题、半角符号、主菜单标题/顺序/显隐；启动文件夹、稳定排序、仅管理文件夹 | P2-01、P2-02 | M1、M2 |
| M5 文本与服务 | Markdown 偏好、盘古间距、代码语言、输入提示；凭据存储；多实例翻译、LLM 模型/提示词、草稿翻译回填、简繁转换；自定义转写；系统翻译/AI 适配 | P2-03 文本部分、P2-05、P2-06 | M0、M3（配置交换） |
| M6 规则类模块 | 正则过滤、作者/已屏蔽来源、Zalgo、对话排除、规则模板交换；链接预览偏好与 URL 修正规则；消息截图 | P2-07、P2-10 链接部分、P2-08 | M3、M5 |
| M7 其余 P2 与交换界面 | 本地别名、演示模式、应用角标、通知延迟、分类反应；贴纸目录备份/排序、GIF 播放控制、MP4 文件预览；配置交换完整界面与诊断 | P2-09、P2-10 其余、P2-11 界面 | M3 |
| P3 专项 | 回执/在线策略、本地历史、内容保护与锁、自动翻译继承与 LLM 上下文、规则继承与远程规则、网络调优与代理、外部媒体后端、云同步 | P3-01 至 P3-09 | 对应 P2 完成；每项先单独写设计并评审 |

M1、M2、M3 之间没有依赖，可并行开发，但各自为独立 change。M3 是旧实现问题最集中的部分，其原型结论需要在本文件记录后再展开实现。

### 里程碑状态

| 里程碑 | 状态 | 上游改动文件数 | 挂钩数 |
| --- | --- | --- | --- |
| M0 | 未开始 | — | — |
| M1–M7 | 未开始 | — | — |

## 6. 待决事项

| 事项 | 建议 | 需要谁决定 |
| --- | --- | --- |
| 旧账号数据（启动文件夹、过滤、别名、管理文件夹）是否迁移 | 不迁移，发布说明中注明；预发布版本用户数量有限，迁移代码需要判别不可靠的尾部字节 | 维护者 |
| 旧代码的复用方式 | 按功能包逐文件审查后移植 `nagram/` 下的纯逻辑模块（解析、校验、服务请求），上游挂钩部分重写 | 维护者 |
| 消息菜单挂钩方式 | 先按第 3.4 节做原型，以实测结果决定 | M3 原型 |
| 界面场景测试是否常驻仓库 | 取决于 M0 对上游测试代理扩展方式的评估 | M0 评估 |
