# ADR-0001：上游关系与最小侵入

- 状态：Accepted
- 日期：2026-09-30

## 背景

仓库 fork 自 NextAlone/Nagram-qt，与其 `main` 完全一致（GitHub compare：ahead 0、behind 0）。Nagram-qt 是 Telegram Desktop `dev` 之上的 67 个提交，用 jj 持续 rebase，并把修复并入原提交，历史会被改写。现有改动触及 192 个上游文件；其中源码 123 个文件、新增 1,386 行。

## 方案比较

| 方案 | 优点 | 缺点 |
| --- | --- | --- |
| A. 继续跟随 Nagram-qt | 直接获得 Nagram 的后续功能 | Nagram 历史被反复改写，每次同步都要 rebase 自有提交；品牌与架构受制于对方；Nagram 品牌政策要求分支替换品牌 |
| B. 以 Telegram Desktop 为唯一上游，Nagram 与 AyuGram 只作参考 | 同步对象唯一且稳定；可自由重构自有代码 | 需要自行实现 Nagram 后续新增的功能 |
| C. 改以 AyuGram Desktop 为基线 | 已有幽灵模式与消息历史 | AyuGram 直接在上游文件中打大量补丁，并把 SQLite、sqlite_orm、nlohmann/json 直接拷进源码树；冒充官方客户端，与本项目非目标冲突 |

## 决定

采用 B。上游固定为 `telegramdesktop/tdesktop` 的 `dev`；Nagram-qt 与 AyuGram 的代码只作为行为参考，按模块重写或移植，不 cherry-pick 其提交。

最小侵入的具体做法：

1. 自有代码全部在 `Telegram/SourceFiles/serein/`、`proto/`、`tools/serein/`、`Telegram/cmake/serein*.cmake` 与 `Telegram/Resources/langs/serein/`。
2. 上游源文件只包含 `serein/hooks/*.h`，每个挂钩一行；上游的 `rpl` 事件能满足时不加挂钩。
3. 品牌与构建文件的上游改动集中在单独的品牌提交中。
4. 自有提交分为“挂钩”和“功能”两类：同步上游时冲突只可能出现在挂钩与品牌提交中。
5. `tools/serein/upstream_budget.py` 在 CI 中统计上游文件数、新增行数与挂钩数，超出预算即失败。

## 后果

- 同步上游的流程是普通的 `git merge` 或 rebase 到新的 `dev`，不再依赖 jj。
- Nagram-qt 中已实现的功能按功能族迁入新结构，旧的内联挂钩在迁移中收敛到门面。
