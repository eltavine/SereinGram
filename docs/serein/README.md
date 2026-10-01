# SereinGram 产品规格

> 状态：Draft 0.1 · 日期：2026-09-30 · 上游基线：`telegramdesktop/tdesktop` 的 `dev`（7.2.10 beta 之后）· 许可证：GPLv3

SereinGram 是基于 Telegram Desktop 的第三方桌面客户端，功能对标手机版 Nagram（iOS、Android）与 AyuGram（Desktop、Android），支持 macOS、Windows 和主流 Linux 发行版。本目录是项目的唯一权威规格：

| 文档 | 内容 |
| --- | --- |
| [功能矩阵](features.md) | 功能 TODO 清单：稳定 Feature ID、来源、状态、优先级 |
| [架构](architecture.md) | 模块分层、依赖方向、上游挂钩门面、schema、存储、守卫与迁移步骤 |
| [ADR](adr/) | 关键取舍与调研记录 |

`docs/nagram/` 保留 Nagram 阶段的需求、手机端功能来源目录与现场验证记录，作为参考资料；与本目录冲突时以本目录为准。

## 0. 全局工程原则

以下原则是架构评审、代码评审、依赖升级和发布验收的强制检查项。例外必须写入 ADR，说明影响范围、补偿措施和到期条件。

1. **极端模块化**：每个模块职责单一，通过最小的稳定接口协作，边界、依赖方向与所有权显式记录。SereinGram 自有的每个源文件不超过 1000 行，由 CI 守卫强制；上游 Telegram Desktop 的文件不受此限制。
2. **低耦合与契约隔离**：功能逻辑只依赖端口（port）和 schema 生成的值类型；上游类型只出现在适配器、界面层和挂钩门面中。替换一个实现不得要求修改无关模块。
3. **面向扩展**：新功能通过注册模块、实现端口或声明 schema 字段接入，不在核心流程中增加条件分支。
4. **隔离 BREAKING CHANGE**：持久化格式、配置交换文件和 schema 必须版本化，`buf breaking` 在 CI 中阻止不兼容变更；上游 API 的变化只允许波及适配器与挂钩门面。
5. **优先采用成熟工具**：优先使用上游已构建的库（Qt、lib_ui、lib_base 等）和现代、广泛采用、持续维护的开源工具；已有合适方案的基础能力禁止自研。确需自研时在 ADR 记录理由、维护成本与退出路径。旧依赖有更好的替代时直接替换；项目处于内部开发期，不做旧数据迁移。
6. **先调研、再决策、后行动**：引入依赖或改变架构前先比较方案（需求适配度、成熟度、维护活跃度、许可证、三平台构建、上游冲突风险），结论写入 ADR。

## 1. 范围

- **目标**：实现功能矩阵中对标手机版 Nagram 与 AyuGram 的增强功能；Telegram 官方功能全部保留并随上游更新；三平台构建与发布。
- **上游**：唯一上游是 Telegram Desktop。Nagram-qt 与 AyuGram 是功能与实现参考，不跟随其提交历史（[ADR-0001](adr/0001-upstream-and-intrusion.md)）。
- **默认行为**：所有增强默认关闭或跟随 Telegram；全部关闭时客户端行为与上游一致。
- **非目标**：冒充官方客户端或使用官方客户端的 API 凭据；伪造服务端权益；手机专属交互（振动、滑动手势、底栏等，排除口径见 `docs/nagram/feature-catalog.md` 末节）。

## 2. 平台矩阵

| 平台 | 产物 | 依赖准备 | 最低系统版本 |
| --- | --- | --- | --- |
| macOS | arm64 + x86_64 通用二进制，DMG | 上游 `prepare.py` + Xcode | 随上游 |
| Windows | x64 安装包与便携版；arm64 随后 | 上游 `prepare.py` + MSVC | 随上游 |
| Linux | x86_64 静态构建 tar 包（Rocky Linux 8 容器，glibc 兼容主流发行版）；发行版打包走 `DESKTOP_APP_USE_PACKAGED` | 上游 Docker 环境 | 随上游 |

## 3. 标识与状态

- Feature ID 形如 `SG-<族>-<序号>`，发布后不复用、不改义。
- 状态：`Planned` 已进入规格、尚无实现；`In Progress` 已开始实现；`Implemented` 代码完成并通过单元测试，现场验收未完成；`Verified` 通过规定的自动化与人工验收。
- 优先级：`P0` 基础设施与发布前提；`P1` 首个版本必须具备；`P2` 随后；`P3` 需要专项设计，或存在服务条款、平台能力风险。

## 4. 发布门禁

1. 三平台 CI 构建成功，`test_serein` 全部通过。
2. 守卫全部通过：源文件行数、模块边界、上游侵入预算、`buf lint` 与 `buf breaking`、生成代码无漂移、三语文案一致、门面命名空间遮蔽、功能矩阵格式、工作流 actionlint。本地用 `tools/serein/check_all.sh` 一次运行全部守卫与核心测试。
3. 发布说明分别列出本版本 `Verified` 与仅 `Implemented` 的功能。
4. 发布流程：推送 `v*` 标签后，三平台工作流构建并把产物（macOS 通用 DMG、Linux x86_64 tar 包、Windows x64 便携版与安装包）上传到同一个草稿预发布 Release，维护者核对门禁后手动发布；应用内的 GitHub 更新检查只提示正式版。CI 目前只构建 Debug 配置，正式版的构建配置与签名由维护者决定。

## 5. 提交规范

- 提交信息遵循 Conventional Commits（`type(scope): summary`），全部使用英文，并且必须有详细正文，说明改动的原因和内容。
- CI 的 `Commit messages` 任务用 commitlint 与 `tools/serein/commitlint.config.mjs` 检查每次推送或 PR 新增的提交：标题不超过 72 字符，正文不少于 60 字符、每行不超过 100 字符，只允许可打印 ASCII。
- 提交前可运行 `python3 tools/serein/check_commit_message.py <信息文件>` 做同样的检查；执行 `git config core.hooksPath tools/serein/githooks` 可在本地启用 `commit-msg` 钩子自动检查。
- 该规则自 2026-09-30 起生效，优先于 `AGENTS.md` 中“标题一行”的约定；此前已推送的提交不改写。
- 只检查主线（first-parent）上的提交：同步上游时检查合并提交本身，随合并进入的上游提交保持原样，不按本规范检查。
