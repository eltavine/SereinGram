# SereinGram 产品规格

> 状态：Draft 0.1 · 日期：2026-09-30 · 上游基线：`telegramdesktop/tdesktop` 的 `dev`（7.2.10 beta 之后）· 许可证：GPLv3

SereinGram 是基于 Telegram Desktop 的第三方桌面客户端，功能对标手机版 Nagram（iOS、Android）与 AyuGram（Desktop、Android），支持 macOS、Windows 和主流 Linux 发行版。本目录是项目的唯一权威规格：

| 文档 | 内容 |
| --- | --- |
| [功能矩阵](features.md) | 功能 TODO 清单：稳定 Feature ID、来源、状态、优先级 |
| [架构](architecture.md) | 模块分层、依赖方向、上游挂钩门面、schema、存储、守卫与迁移步骤 |
| [ADR](adr/) | 关键取舍与调研记录 |

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
- **非目标**：冒充官方客户端或使用官方客户端的 API 凭据；伪造服务端权益；手机专属交互（振动、滑动手势、底栏等，排除口径见[功能矩阵](features.md#明确排除)末节）。

## 2. 平台矩阵

| 平台 | 产物 | 依赖准备 | 最低系统版本 |
| --- | --- | --- | --- |
| macOS | arm64 + x86_64 通用二进制，DMG | 上游 `prepare.py` + Xcode | 随上游 |
| Windows | x64 安装包与便携版；arm64 随后 | 上游 `prepare.py` + MSVC | 随上游 |
| Linux | x86_64 静态构建 tar 包、AppImage、`.deb` 与 `.rpm`（Rocky Linux 8 容器，glibc 2.28 及以上）；Arch Linux PKGBUILD 与 Flatpak 清单以系统库构建 | 上游 Docker 环境；Arch 与 Flatpak 用各自的依赖 | 随上游 |

Linux 发行版打包：

- `packaging/nfpm/`：把静态构建打成 `.deb` 与 `.rpm`，Linux 工作流随构建产物一起生成。
- `packaging/arch/PKGBUILD`：以系统库构建 `sereingram-desktop-git`，CI 工作流 `serein-arch.yml` 在 Arch 容器中构建并安装检查。
- `packaging/flatpak/`：GNOME 运行时上的 Flatpak 清单，构建本地检出，CI 工作流 `serein-flatpak.yml` 生成 `.flatpak` 包。
- 打包者必须使用自己的 API 凭据：PKGBUILD 读取环境变量 `SEREIN_API_ID` 与 `SEREIN_API_HASH`，Flatpak 读取被忽略的 `Telegram/build/api_credentials.local.cmake`；缺少凭据时构建报错。以系统库构建时不检查 GitHub 更新，由包管理器负责更新。
- `tools/serein/check_packaging.py` 要求两份配方锁定的依赖版本与 `snap/snapcraft.yaml` 一致。`upstream_sync.py` 合并上游后自动改写两份配方中 tdlib、tg_owt、tlottie 与 patches 的提交（也可运行 `check_packaging.py --update`）；Qt 版本变化需要新的源码包校验值，tlottie 提交变化需要重新生成 `tlottie-cargo-sources.yml`，这两项由检查报出后手动更新。

## 3. 标识与状态

- Feature ID 形如 `SG-<族>-<序号>`，发布后不复用、不改义。
- 状态：`Planned` 已进入规格、尚无实现；`In Progress` 已开始实现；`Implemented` 代码完成并通过单元测试，现场验收未完成；`Verified` 通过规定的自动化与人工验收。
- 优先级：`P0` 基础设施与发布前提；`P1` 首个版本必须具备；`P2` 随后；`P3` 需要专项设计，或存在服务条款、平台能力风险。

## 4. 发布门禁

1. 三平台 CI 构建成功，`test_serein` 全部通过。
2. 守卫全部通过：源文件行数、模块边界、上游侵入预算、`buf lint` 与 `buf breaking`、生成代码无漂移、三语文案一致、门面命名空间遮蔽、功能矩阵格式、工作流 actionlint。本地用 `tools/serein/check_all.sh` 一次运行全部守卫与核心测试。
3. 发布说明分别列出本版本 `Verified` 与仅 `Implemented` 的功能。
4. 发布流程：推送 `v*` 标签后，三平台工作流构建并把产物（macOS 通用 DMG、Linux x86_64 tar 包、AppImage、`.deb` 与 `.rpm`、Windows x64 便携版与安装包）上传到同一个草稿预发布 Release，核对门禁后手动发布；应用内的 GitHub 更新检查只提示正式版。CI 目前只构建 Debug 配置，正式版的构建配置与签名尚未确定。

## 5. 提交规范

- 提交信息遵循 Conventional Commits（`type(scope): summary`），全部使用英文，并且必须有详细正文，说明改动的原因和内容。
- CI 的 `Commit messages` 任务用 commitlint 与 `tools/serein/commitlint.config.mjs` 检查每次推送或 PR 新增的提交：标题不超过 72 字符，正文不少于 60 字符、每行不超过 100 字符，只允许可打印 ASCII。
- 提交前可运行 `python3 tools/serein/check_commit_message.py <信息文件>` 做同样的检查；执行 `git config core.hooksPath tools/serein/githooks` 可在本地启用 `commit-msg` 钩子自动检查。
- 该规则优先于 `AGENTS.md` 中“标题一行”的约定。
- 只检查主线（first-parent）上的提交：同步上游时检查合并提交本身，随合并进入的上游提交保持原样，不按本规范检查。

## 6. 界面翻译

- Telegram 自身的字符串沿用官方翻译平台与语言包；Serein 新增的字符串（`lng_serein_*`）在 `Telegram/Resources/langs/serein/`。
- `serein.strings` 是英文源文件；译文文件名为小写语言代码，例如 `fa.strings`、`pt-br.strings`。CMake 在配置时扫描该目录生成资源清单，新增语言无需改动构建文件。
- 运行时先按界面语言的完整代码查找译文，再退回基础语言代码；缺少的键显示英文。`zh-hans` 与 `zh-hant` 必须完整，其他语言可以只翻译一部分。
- 核心测试校验每个译文文件：键必须存在于英文源文件，`{name}` 形式的占位符必须一致。
- 仓库根目录的 `crowdin.yml` 把该目录接入 Crowdin；创建 Crowdin 项目后，在 GitHub Secrets 中设置 `CROWDIN_PROJECT_ID` 与 `CROWDIN_PERSONAL_TOKEN`。工作流 `serein-crowdin.yml` 在英文源文件改动推送到 develop 时上传源文件，每周一下载译文并向 develop 提交 PR，也可手动运行；未设置这两个机密时直接跳过。
