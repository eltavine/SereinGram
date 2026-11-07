# ADR-0003：消息历史存储

- 状态：Accepted（有验证门槛）
- 日期：2026-09-30

## 背景

SG-HIST 需要按账号保存已删除消息与编辑历史，并支持按对话查询、按时间排序、保留期限与清理。数据包含消息正文，属于敏感数据；上游的 `tdata` 以本地密钥加密。

## 方案比较

| 方案 | 优点 | 缺点 |
| --- | --- | --- |
| A. AyuGram 做法：源码树内拷贝 `sqlite3.c` 与 `sqlite_orm.h` | 已在 AyuGram 验证 | 拷贝的第三方代码无法随上游更新；数据库未加密 |
| B. Qt SQL + Qt 自带的 SQLite 驱动 | 上游静态 Qt 已构建 Qt SQL（`prepare.py` 未关闭），零新增依赖；三平台一致 | 不支持整库加密 |
| C. SQLCipher 或 SQLite3 Multiple Ciphers | 整库加密 | 新增依赖，需接入三平台构建 |
| D. 上游 `lib_storage` 缓存数据库 | 已加密、随账号生命周期 | 只有键值接口，按对话查询与排序需要自建索引 |

## 决定

采用 B，并对载荷加密：

- 每个账号一个数据库文件，位于该账号的数据目录 `serein_history.sqlite3`。
- 明文列只保留查询所需的元数据：对话 ID、消息 ID、版本号、类型（删除、编辑）、时间。
- 载荷是 `serein.history.v1.HistoryRecord` 的 JSON（含原始 TL 字节与 API layer），用由账号本地密钥派生的密钥加密后写入。
- 渲染时按当前 schema 解析原始 TL，成功就用上游的消息构造路径还原原始消息，完整支持所有 Telegram 消息类型；解析失败时退回记录中的文本、实体与媒体摘要。（2026-10-05 修订：原定以 layer 相等为条件。TL 构造器编号是其声明的校验和，字段一变编号就变，旧 layer 中未改动的构造器仍能正确解析，改动过的必然解析失败；以 layer 相等为条件会让每次 layer 升级都丢掉全部媒体。layer 仍随记录保存，用于诊断。）
- 迁移用 `PRAGMA user_version`；每个格式版本保留不可变的测试样本。

验证门槛：在三平台 CI 中确认 `Qt6::Sql` 与 SQLite 静态插件可用。任一平台不可用时，改用方案 C 中的 SQLite3 Multiple Ciphers，并以子模块方式接入。

实施记录：端口 `serein/ports/history_store.h`（存储与加密接口）和适配器 `serein/adapters/qtsql/history_store.*` 已实现，Homebrew Qt 6.11.2 与 CI 的 Linux 核心测试覆盖保存、分页、版本、裁剪、损坏行与新版本 schema 拒绝。主构建只在找到 `Qt6::Sql` 时给 `test_serein` 编译并运行存储测试，配置日志会打印结果。主程序找到 `Qt6::Sql` 时链接它，由组合根 `serein/app/history_storage.cpp` 把适配器登记为历史存储；`features/history` 只依赖端口，没有 Qt SQL 的构建中历史功能保持关闭。

实施记录（原始 TL 与聊天分区）：上游 `History::createItem` 与 `Session::updateEditedMessage` 各一行挂钩把收到与编辑后的 `MTPMessage` 序列化后交给 `features/history/wire_cache.cpp`，缓存按会话持有、消息被移除时一并丢弃，删除与编辑时随记录写入载荷的 `tl_message` 与 `api_layer`。还原经上游 `AdminLog::PrepareLogMessage`（为此移出匿名命名空间并在头文件声明）去掉回复、相册分组、自毁计时与浏览数后交给 `History::createItem`：聊天分区中的副本与管理日志一样使用非历史消息 ID 并带 `MessageFlag::AdminLogEntry`，不进入对话的消息块与已读计算，销毁时也不会取消共享文件的下载；原位保留的副本按原消息的方向标记还原。端口提供按 `(消息 ID, 版本)` 的键集分页 `records()` 与计数 `count()`，纯模型 `features/history/model/window.cpp` 计算以某条消息为中心的窗口并有核心测试；列表、单对话清理、被移出对话的保存与缓存媒体都不再有条数或大小上限，只有用户设置的保留天数与总条数上限会裁剪记录。

## 后果

- 元数据（谁、何时、在哪个对话）以明文存在本机；需要整库加密时按方案 C 升级，载荷格式不变。
- 退出账号、清理历史必须删除对应数据库文件并在失败时提示，不能以界面消失代替清理。
