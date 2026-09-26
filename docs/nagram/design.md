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
| 上游挂钩密度过高 | 85 个上游源文件引用 `nagram/` 头文件，662 处 `Nagram::` 调用；其中 `history_view_context_menu.cpp` 138 处、`history_inner_widget.cpp` 130 处、`history_widget.cpp` 56 处 | 上游已前进 207 个提交（含 7.2.9），这些热点文件是上游最常改动的文件，rebase 冲突成本高 | 挂钩尽量沿用上游判断与调用位置；每个里程碑统计上游新增行数并说明集中改动的原因（第 3.4 节） |
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

`Storage::Account` 上游只提供 `bool` 偏好特化；Nagram 在自己的 `options.cpp` 中补充 `QByteArray` 特化，继续使用上游账号 KV 和保存生命周期。

响应式通知由 Nagram 自己维护的 `rpl::event_stream<std::string_view>` 提供，在 `Nagram::Set` 内部触发，不修改上游 `Core::Settings`。旧实现为原子导入新增的 `Core::Settings::applyPrefChanges()` 不再需要：导入先完成全部校验与冲突检查，再在同一事件循环内逐键写入，写入期间抑制通知，结束后统一推送一次。若 M0 验证发现上游逐键写入会产生可观察的中间状态，再以最小改动补一个批量接口。

结构化对象一律为 `{"version": N, ...}`，边界严格校验类型、枚举、范围和未知字段；解析失败保留原字节并在诊断中报告。

### 3.4 上游挂钩规则

1. 上游文件允许新增 `#include "nagram/..."`、在已有判断表达式中增加条件、加入单行 `Nagram::` 调用。功能逻辑放在 `nagram/`；需要调用上游类私有方法时，允许保留约 10 行以内的短逻辑块，并在对应提交正文中说明原因。
2. 优先使用上游已有扩展点：`rpl` 事件、`Data::Session` 通知、样式常量、`Window::SessionController` 生命周期、设置页的 `Settings::Section` 注册。
3. 两套消息列表实现（`HistoryInner` 与 `HistoryView::ListWidget`）只能通过同一个 `nagram/` 入口挂钩，不在两处重复写逻辑。
4. 每个里程碑结束时统计上游文件数、新增行数和挂钩数，写入第 5 节；集中或超过预期的改动须说明原因，不设每个文件只能改一行的目标。
5. 品牌相关改动（应用名、图标、平台标识）集中在一个 change 中，不与功能混合。

**消息菜单（E01–E23）方案**：旧实现在两个菜单填充函数中插入了约 270 处调用。当前方案分两步：

- 在上游创建菜单项的位置用一行 `Nagram::Menu::Tag(action, MenuAction::Reply)` 标记动作身份（不改变上游逻辑与顺序）。
- 上游填充完成后调用一次 `Nagram::Menu::Apply(menu, context)`，按注册表执行隐藏、修饰键显示、分隔线清理，并在固定位置插入 Nagram 新增动作（复读、无引用转发、合并等）。上游已有动作的顺序保持不变。权限与可用性仍由上游创建逻辑决定：未被上游创建的动作不会被 Nagram“恢复”。

原型结论及维护者选择见下。

#### 消息菜单 API 原型结论（2026-09-27）

对两条现有填充路径 `HistoryInner::showContextMenu()` 和 `HistoryView::FillContextMenu()` 做了源码级 API 原型核对。`Ui::PopupMenu::actions()` 可以枚举已填充的 `QAction`，`removeAction(index)` 能隐藏既有项，`insertAction(index, widget)` 能把**新建**的 Nagram 项插到指定位置。`Ui::Menu::removeAction()` 会销毁该项的 `ItemBase`，并在 `QAction` 由菜单持有时销毁 `QAction`；`insertAction()` 只接受新的 `base::unique_qptr<ItemBase>`。公开接口没有取出或移动既有 `ItemBase` 的方法，因而原方案的 `Tag` + 填充后 `Apply` 不能保留原动作及回调完成任意重排。只设置 `QAction::visible` 也不会从 `Ui::Menu` 的布局列表中移除对应控件。

结论：填充后显隐和插入新增动作可行，既有动作的排序不可按原方案实施。设计中建议退回的“填充入口一个过滤器”也无法拦截各处对 `PopupMenu::addAction()` 的直接调用，因此不能据此保证一行挂钩。本次未修改 `lib_ui`，也未做运行态菜单注入。维护者选择 B：不重排上游已有动作；每项只提供“显示／隐藏／按住 Option（Alt）时显示”三态。E15–E17 固定插在“转发”之后，其他 Nagram 新增动作固定插在菜单末尾、“删除”之前；没有对应锚点时插在末尾。S40 按 `Tag` + `Apply` 实施。

### 3.5 文案

- 英文放 `Telegram/Resources/langs/nagram/nagram.strings`，简繁放同目录 `zh-hans.strings`、`zh-hant.strings`，键统一以 `lng_nagram_` 开头。
- 构建时由 CMake 自定义命令把上游 `lang.strings` 与 `nagram.strings` 拼接到构建目录，再交给上游 `generate_lang`；上游 `lang.strings` 保持原样。`td_lang.cmake` 只改输入路径一行。
- 简繁内置文案作为缺失键的后备值，通过 `Lang::Instance` 的一个挂钩注入；不写入云端语言缓存，不覆盖语言包中显式提供的翻译。
- `test_nagram` 检查三语键集合一致、占位符一致、无重复键。

### 3.6 品牌

已随提交 `feat: Nagram 品牌与应用标识` 移植：应用名 Nagram、应用 ID `xyz.nextalone.nagram.desktop`、图标资源、Windows 安装脚本、snap 配置、关于页与托盘文案；关闭上游自动更新与崩溃上报。品牌说明见仓库根目录 `BRANDING.md`。

### 3.7 测试

- **`test_nagram`**：参照上游 `Telegram/cmake/tests.cmake` 新增可执行测试，只链接 `lib_base` 与 `nagram/core` 等纯逻辑代码。覆盖注册表默认值、类型校验、JSON 结构化对象、作用域解析、配置交换、正则过滤预算、盘古间距与实体偏移、URL 规则、文案一致性。在 `DESKTOP_APP_TEST_APPS` 下构建，CI 运行。
- **界面场景**：上游已有 `Test::` 测试代理（Debug 构建加 `-testagent`，使用标记为测试的一次性数据目录）。M0 评估能否让 Nagram 场景以独立文件注册、按环境变量选择，而不是像旧实现那样临时修改 `test_scenario.cpp` 后再删除。
- 每个功能包的验收沿用需求第 5 节；外部服务测试使用 localhost 桩，不向真实聊天发送消息。

### 3.8 版本控制与发布

- `nagram-next` 是建立在 `dev@upstream` 之上的一串 change：`docs` → `build/branding` → `core` → 各功能包。
- 同步上游：`jj git fetch --remote upstream` 后 `jj rebase -b nagram-next -d dev`，逐个 change 解决冲突并跑 `test_nagram`。
- 发行包只上传 GitHub Releases；`releases/` 等本地产物不进入仓库。
- CI 使用 `.github/workflows/nagram-{mac,win,linux}.yml`（由上游同名工作流改写，产物为 Nagram，未配置 Secrets 时使用上游测试凭据）；上游原有工作流在 GitHub 仓库设置中停用，不修改其文件以免 rebase 冲突。CI 改动单独成 change；不提交空提交来触发构建。

## 4. 与旧版本的兼容

不做数据迁移（维护者决定，2026-09-26）。旧版 `v0.1.0-pre.1` 是预发布版本：

- 本机偏好、配置交换文件、系统凭据中的服务密钥均不读取旧格式。新注册表的键名可以与旧版相同，但不为旧值做兼容处理。
- 旧版写在 `SessionSettings` 尾部的账号数据不清理。继续使用旧数据目录时，若上游以后在同一位置追加字段，可能把这些旧字节读成上游设置。发布说明要求从旧版升级的用户使用新的数据目录并重新登录。

## 5. 实施路线

<a id="roadmap"></a>

优先级沿用需求中的 P1 / P2 / P3 分级（常用度 × 复杂度 × 依赖），按依赖关系重新编排为里程碑。提交按设置页的小分组划分，具体步骤、提交信息与编译验证点见 [分步实施计划](implementation-plan.md)；设置页条目见 [设置页设计](settings-page.md)，每个条目的上游改动见 [上游处理点](upstream-hooks.md)。

| 里程碑 | 内容 | 依赖 |
| --- | --- | --- |
| M0 基础设施 | 文档；文案管线；品牌；构建配置与 CI；`nagram.cmake` 与 `test_nagram`；选项注册表；本机与账号存储；简繁内置文案；设置入口与首页 | 无 |
| M1 消息显示 | 分栏“消息”中时间、标记、反应、特效、内容显示五组 | M0 |
| M2 列表、输入、媒体、资料 | 分栏“聊天列表”“输入与发送”“媒体与贴纸”“隐私与资料”中的纯显示与确认类分组 | M0 |
| M3 消息菜单 | 菜单原型验证、标记与后处理、Nagram 新增动作、结构化配置核心 | M0 |
| M4 界面与导航 | 分栏“界面”，启动文件夹、会话排序、管理文件夹 | M1、M2 |
| M5 文本与服务 | 文本格式、阅读投影、服务实例与凭据、翻译、转写、系统 AI | M0、M3 |
| M6 规则与截图 | 消息过滤、链接规则、消息截图 | M3、M5 |
| M7 其余与配置管理 | 演示模式、本地别名、GIF 与 MP4、贴纸目录、配置管理 | M3 |
| P3 专项 | 回执/在线策略、本地历史、内容保护与锁、自动翻译继承与 LLM 上下文、规则继承与远程规则、网络调优与代理、外部媒体后端、云同步 | 对应 P2 完成；每项先单独写设计并确认 |

M1、M2、M3 之间没有依赖，但按计划顺序提交，避免并行分支的 rebase 成本。M3 的原型结论记录在本文件后再展开实现。

### 里程碑状态

| 里程碑 | 状态 | 上游改动文件数 | 挂钩数 |
| --- | --- | --- | --- |
| M0 | S00–S14 已提交；macOS arm64 Debug clean／增量构建及 `test_nagram` 通过。独立数据目录登录后的设置首页、搜索、125%／200% 缩放和英／简／繁文案已检查；双账号隔离未验证，三平台 CI 等首次获准推送 | 相对 `dev` 共 122 个文件；其中原有 Telegram 文件 81 个（文本／代码 35、资源 46） | 功能调用 2 处（语言、设置）；另有构建与样式接入 |
| M1 | S20–S24 已提交；V2 部分完成：macOS arm64 Debug 增量构建与 `test_nagram` 通过；独立数据目录中的 23 个开关均完成开／关／恢复默认，C09 文本恢复默认，现场效果与未验证场景见下表。设置搜索跳转高亮、125%／200% 下英／简／繁首页及消息页布局通过。双账号隔离未验证；三平台 CI 等首次获准推送 | 相对 `dev` 共 157 个文件；其中原有 Telegram 文件 99 个（源码 47、资源 48、其他 4） | M1 新增 43 行调用／条件／`friend` 标记（按上游 `SourceFiles` 的 diff 中新增 `Nagram::` 行计） |
| M2 | S30–S3B 已实现；macOS arm64 Debug 增量构建与 `test_nagram` 通过，单账号现场结果见下表。D22、D23、D26 补测通过；其他缺少样本或自动化能力的交互、双账号隔离与三平台 CI 仍未验证；未推送 | 相对 `dev` 共 218 个文件、其中原有 Telegram 文件 137 个；M2 增量 75 个文件、其中原有 Telegram 文件 50 个 | M2 在上游 `SourceFiles` 中新增 152 行含 `Nagram::` 的调用／条件；`history_widget.cpp`、`history_view_compose_controls.cpp`、`stickers_list_widget.cpp` 最集中。输入按钮原本分布在不同判断处，保留就地条件可降低上游同步冲突；D14 的命令草稿分支及按钮刷新订阅需要调用 `HistoryWidget` 私有方法，保留短逻辑块 |
| M3 | 已完成：消息菜单 API 原型与 S40–S44；macOS arm64 Debug 构建及 `test_nagram` 通过。用户在新构建中复查 A–E、气泡外右键及 `+1` 图标均通过，开关已恢复默认，应用已退出。双账号隔离与三平台 CI 未验证 | 相对 `dev` 共 240 个文件；M3 增量 36 个文件，其中 4 个上游 `SourceFiles` 文件和 3 个 `+1` 图标资源 | M3 上游 `SourceFiles` 新增 95 行，含 65 行 `Nagram::` 挂钩或声明；菜单动作分散在两个上游填充路径，逐项 `Tag` 符合选定的 B 路线 |
| M4–M7 | 未开始 | — | — |

### M1 V2 现场核验（2026-09-27）

仅使用独立目录 `~/NagramTest/profile1` 的单个已登录测试账号。所有布尔开关均完成 0→1→0 的界面往返；测试后重新启动，23 个开关均为 0，C09 显示 `Default`，语言恢复 English，界面缩放恢复自动 110%，客户端已退出。下表的“未验证”指缺少能观察该功能实际效果的消息或场景，开关往返仍通过。话题聊天、双账号隔离和三平台 CI 均未验证。

| 条目 | 实际效果核验 |
| --- | --- |
| C01 | 通过：已打开频道的时间即刻增减秒数，关闭恢复；`out/nagram-v2/c01-on-channel.png`、`c01-off-channel.png` |
| C02 | 未验证：缺少可核对原始时间的转发消息 |
| C03 | 未验证：缺少适合观察的服务消息 |
| C04 | 未验证：未取得时间提示中的消息 ID 截图 |
| C05 | 未验证：未核对精确浏览数及回复数 |
| C06 | 通过：已打开频道的浏览数即刻隐藏，关闭恢复；`out/nagram-v2/c06-on-channel.png` |
| C07 | 未验证：缺少带频道签名的样本 |
| C08 | 未验证实际消息：设置页中 C09 随开关隐藏／重现通过 |
| C09 | 未验证实际消息：自定义文字保存与恢复 `Default` 通过；`out/nagram-v2/c09-custom-settings.png`、`c09-default-restored.png` |
| C10 | 通过（频道）：反应栏即刻隐藏并重排，关闭恢复；从属 C11–C13 禁用且无法点击；私聊、群组和话题效果未验证；`out/nagram-v2/c10-on-channel.png`、`c10-disabled.png` |
| C11–C12 | 未验证：私聊、群组反应样本未检查 |
| C13 | 通过（频道）：单独开启后反应栏即刻隐藏，关闭恢复；`out/nagram-v2/c13-isolated-on.png`、`c13-off-immediate.png` |
| C14–C15 | 未验证：未观察右键和选中消息时的反应面板 |
| C16–C18 | 未验证：缺少贴纸、表情互动和消息特效样本 |
| C19 | 未验证：缺少剧透消息样本 |
| C20 | 通过（频道）：快捷转发按钮即刻隐藏，关闭恢复；`out/nagram-v2/c20-on-channel.png` |
| C21 | 未验证：缺少推荐频道入口样本 |
| C22 | 通过（聊天列表）：私聊行的 Premium 表情状态隐藏，认证标记保留；关闭恢复；其他资料位置未验证；`out/nagram-v2/c22-on-channel-list.png` |
| C23 | 未验证：未打开收藏夹聊天 |
| C24 | 未验证：未观察到私聊对方正在输入 |

设置搜索 `Nagram` 与 `Hide view counts` 均能跳转，后者高亮目标行；证据在 `out/nagram-v2/search-*.png`。125% 与 200% 下 English、简体中文、繁體中文各有首页和消息页截图，命名为 `out/nagram-v2/scale-{125,200}-{home,messages}-{en,zh-hans,zh-hant}.png`；首屏未见裁切或错误换行。200% 繁體中文下 C10 的从属开关禁用状态见 `out/nagram-v2/scale-200-child-disabled-zh-hant.png`。截图是本机 `out/` 中的临时证据，不随提交发布。

### M2 V2 现场核验（2026-09-27）

仅使用 `~/NagramTest/profile1` 的单个测试账号。补测只向收藏夹发送了 1 条文字、2 张贴纸和 2 个 GIF；私聊通话确认框只点“取消”，没有拨出，也没有执行付费和管理操作。S30–S3B 的布尔开关已完成开启／关闭往返，非布尔选项已恢复默认；补测临时开启的 6 个开关也已核对为关闭。下表评定的是**实际效果**；“未验证”不表示开关写入失败。没有第二个账号，双账号隔离未验证；三平台 CI 等获准推送后运行。收藏夹 5 条测试消息的清理状态见表后说明。

| 条目 | 结果 | 证据或缺口 |
| --- | --- | --- |
| B01 | 通过 | 列表紧凑布局切换后即时重排，关闭恢复；`out/nagram-v2/s30-list-hidden-valid.png` |
| B02 | 通过 | 预览行数切换后列表即时重排并恢复；`out/nagram-v2/s30-list-hidden-valid.png` |
| B03 | 通过 | 收藏夹／归档预览即时隐藏并恢复；`out/nagram-v2/s30-list-hidden-valid.png` |
| B04 | 通过 | 动态条即时隐藏、收起并恢复；按维护者决定保留内部对象；`out/nagram-v2/s30-list-hidden-valid.png` |
| B06 | 通过 | “全部会话”侧栏项在仍有其他文件夹时即时隐藏并恢复 |
| B07 | 通过 | 文件夹中的归档入口即时出现并恢复；`out/nagram-v2/s31-folders-on.png` |
| B08 | 通过 | 文件夹未读数即时隐藏并恢复 |
| B10 | 未验证 | 测试账号缺少赞助消息和搜索广告样本；`out/nagram-v2/s32-promotions-on.png` |
| B11 | 未验证 | 缺少代理赞助频道与重新连接代理场景 |
| B12 | 未验证 | 缺少 Premium 推广提示样本 |
| B13 | 未验证 | 缺少生日提示样本 |
| B14 | 未验证 | 未执行滚动到底后的频道切换手势；`out/nagram-v2/s33-scroll-navigation-on.png` |
| B15 | 未验证 | 测试账号没有可用的话题切换场景 |
| D01 | 通过 | 已打开聊天的附件按钮即时隐藏并恢复 |
| D02 | 通过 | 已打开聊天的表情按钮即时隐藏并恢复 |
| D03 | 通过 | 已打开聊天的录音按钮即时隐藏并恢复 |
| D04 | 未验证 | 当前聊天未显示命令按钮；`out/nagram-v2/s34-compose-hidden.png` |
| D05 | 未验证 | 缺少机器人菜单按钮场景 |
| D06 | 未验证 | 缺少自动删除按钮场景 |
| D07 | 未验证 | 缺少输入区礼物按钮场景 |
| D08 | 未验证 | 缺少输入区 AI 按钮场景 |
| D09 | 未验证 | 缺少发送身份切换按钮场景 |
| D10 | 未验证 | 缺少 Stars 反应按钮场景 |
| D11 | 未验证 | 缺少频道底部静音按钮可切换样本 |
| D12 | 未验证 | 未完整复现悬停唤出表情面板 |
| D13 | 未验证 | 未完整复现悬停唤出附件菜单 |
| D14 | 未验证 | 未操作机器人命令以免触发发送 |
| D15 | 通过 | 已打开聊天的占位文字在默认／对话名称／发送身份之间即时切换，最后恢复默认；`out/nagram-v2/s35-placeholder-chat.png`、`s35-placeholder-sender.png` |
| D22 | 通过 | 向收藏夹点贴纸后出现确认框；确认后只发送一次且弹框关闭；`out/nagram-v2/m2-d22-sticker-confirm-fixed.png` |
| D23 | 通过 | 从 GIF 面板选择收藏的 GIF 后出现确认框；确认后只发送一次且弹框关闭；`out/nagram-v2/m2-d23-gif-confirm-fixed.png` |
| D24 | 未验证 | 已开启选项；界面自动化未能完成持续按住录音键，未取得发送前试听画面 |
| D25 | 未验证 | 已开启选项；界面自动化未能完成持续按住录像键，未取得发送前预览画面 |
| D26 | 通过 | 私聊点击通话后显示确认框，随后点“取消”；未拨出；`out/nagram-v2/m2-d26-private-call-confirm.png` |
| D27 | 未验证 | 消息操作菜单在界面自动化中未能打开，未实际转发到收藏夹；`out/nagram-v2/s37-forward-order-on.png` |
| F01 | 通过 | 已打开私聊里的贴纸在 150%／100% 间即时缩放；`out/nagram-v2/s38-sticker-150.png`、`s38-sticker-100.png` |
| F02 | 通过 | 贴纸时间即时隐藏，发送状态仍显示，关闭恢复；`out/nagram-v2/s38-hide-sticker-time.png` |
| F03 | 未验证 | 数值写入与默认恢复通过，未核对最近使用列表实际截断 |
| F04 | 未验证 | 缺少可辨识的群组贴纸样本；`out/nagram-v2/s38-media-filters-on.png` |
| F05 | 未验证 | 缺少可辨识的推荐贴纸样本 |
| F06 | 未验证 | 缺少可辨识的推荐表情样本 |
| F07 | 未验证 | 缺少可辨识的 GIF 推荐分类样本 |
| F08 | 未验证 | 缺少新私聊问候贴纸场景 |
| F09 | 未验证 | 开关往返通过，缺少视频与圆形视频样本；`out/nagram-v2/s39-video-autoplay-on.png` |
| G01 | 未验证 | 复用上游账号设置，开关往返通过；未取得遮盖手机号的视觉对照；`out/nagram-v2/s3a-privacy-on.png` |
| G03 | 未验证 | 缺少已读时间提示样本 |
| G04 | 未验证 | 缺少分享手机号提示样本 |
| G05 | 通过 | 已打开资料页即时显示 ID；频道原始格式为正数，Bot API 格式有 `-100` 前缀，关闭后消失；`out/nagram-v2/s3b-channel-raw.png`、`s3b-channel-bot-api.png` |
| G06 | 通过 | 已打开资料页即时显示头像已有的 DC 信息，关闭后消失；`out/nagram-v2/s3b-profile-id-dc-gifts-hidden.png` |
| G07 | 通过 | 既有礼物的私聊资料页礼物按钮与礼物行即时隐藏，关闭后恢复；`out/nagram-v2/s3b-profile-default.png`、`s3b-profile-id-dc-gifts-hidden.png`、`s3b-profile-restored.png` |
| G08 | 未验证 | 当前账号没有可辨识的“创建待办清单”菜单入口；设置开关往返通过 |

截图保存在本机 `out/nagram-v2/`，不随提交发布。S30 误拍到其他应用的旧截图已丢弃，并补拍 `s30-list-hidden-valid.png`。收藏夹在补测期间新增的 5 条消息尚未删除：04:08 的测试文字、04:10／04:17 的贴纸、04:17／04:22 的 GIF。右键及聊天菜单均未通过界面自动化打开；已请用户按这些时间与类型手动删除，不能用清空会话代替逐条清理。M2 的三平台 CI、双账号隔离、话题相关功能及上表未验证场景仍需补齐。

### M3 V2 核验（2026-09-27）

本地 `dev` 已是 M3 提交链的祖先。`out/nagram-debug` 完整清理重建通过，日志在 `out/nagram-v2/m3-full-build.log`；`test_nagram` 通过设备与账号选项、菜单三态和版本化配置交换、166 个英文文案键的检查。配置交换测试覆盖未知键跳过、非法值拒绝、差异预览冲突和批量写入后通知。现场发现设置搜索索引在无界面控制器时解引用空指针，已改用 `builder.session()`；增量 Debug 构建及 `test_nagram` 再次通过，搜索页可打开。

| 范围 | 结果 | 证据或缺口 |
| --- | --- | --- |
| E01–E14 上游菜单项三态显隐 | 部分通过 | 用户在 `profile1` 中手动确认：默认菜单与上游一致；“回复”隐藏后消失且分隔线正常，Option 条件显示在松开和按住时分别生效；话题或计划消息页路径同样生效。其余菜单项未逐项检查 |
| E15 复读菜单位置 | 通过 | 用户手动确认设为显示后出现在“转发”之后；只查看，未触发发送 |
| E16–E17 无引用动作、E24 复读确认 | 未验证 | 默认隐藏或关闭；未在收藏夹触发发送，无新增测试消息 |
| E18 批量预览与草稿 | 未验证 | 未打开消息选择和预览界面 |
| E19 选择同一发送者 | 未验证 | 未检查该新增项的出现条件和选择结果 |
| E20 媒体信息 | 未验证 | 未打开媒体消息右键菜单 |
| 气泡外右键 | 通过 | 用户在新构建中手动复查普通聊天与收藏夹：气泡外空白处右键只有“选择” |
| 复读图标 | 通过 | 用户在新构建中手动复查：气泡内“复读”位于“转发”之后并显示 `+1`；设置页“复读”“无引用复读”显示 `+1`，“无引用转发”显示转发图标；未触发发送 |
| 设置搜索 | 通过 | 搜索 `Nagram` 可跳转首页；搜索 `Repeat as copy` 可跳转消息菜单，并滚动、高亮 `Repeat without attribution` 行 |
| S44 配置交换核心 | 通过 | `test_nagram` 的本机设置、菜单结构化 JSON、非法值和冲突测试通过；按计划此步无导入导出界面 |
| 双账号隔离 | 未验证 | 指定测试目录只有一个账号 |
| 三平台 CI | 未验证 | 未获准推送，未运行远端工作流 |

自动化检查仅使用 `~/NagramTest/profile1`，当时 E01 已恢复 `Show`，未发送新消息。之后用户在新构建中手动完成 A–E、气泡外菜单和图标复查，确认开关恢复默认、应用退出。M2 留下的 5 条收藏夹测试消息仍待逐条清理，时间与类型见上一节。M3 没有保存到本地的现场截图；完整构建日志在 `out/nagram-v2/`，不随提交发布。表中未实际操作的单项仍保留未验证标记。

## 6. 已确认的决定

| 事项 | 决定 |
| --- | --- |
| 旧版数据迁移 | 不迁移 |
| 旧代码复用 | 不要求严格复用，按模块参考（见实施计划第 1 节） |
| Nagram 设置入口 | 设置主页顶部 |
| 消息菜单中 Nagram 新增项 | 默认隐藏 |
| 提交粒度 | 按设置页小分组提交 |
| 构建与 CI | 沿用上游工作流，API 凭据来自环境变量、仓库 Secrets 或不提交的本地文件 |
| 编译验证 | 计划中设置阶段性编译验证点 |
| 推送 | 暂不推送 |
| A01 自选等宽字体 | 放弃；不维护 `lib_ui` fork，不注册该设置（维护者决定，2026-09-26） |

## 7. 待决事项

| 事项 | 建议 | 需要谁决定 |
| --- | --- | --- |
| 消息菜单挂钩方式 | 选择 B：保留上游动作顺序，使用 `Tag` + 填充后 `Apply` 做三态显隐、分隔线清理和固定位置插入（第 3.4 节） | 已确认，2026-09-27 |
| 界面场景测试是否常驻仓库 | 可以。`Test::Start()` 已在 Debug `-testagent` 下调用 `SetupScenario()`，并在运行阶段强制检查独立数据目录的 `testing` 标记。Nagram 场景可放在 `nagram/tests/` 的独立源文件中，由 `nagram.cmake` 编译，使用环境变量选择场景；在 `test_runner.cpp` 增加一个 include 和一行注册调用即可，不必改写常驻的 `test_scenario.cpp`。实际注册与场景随首个界面功能提交。 | M0 评估完成；首个界面功能验证时实施 |
