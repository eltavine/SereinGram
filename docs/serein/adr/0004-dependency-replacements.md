# ADR-0004：用成熟依赖替换自研实现

- 状态：Accepted（各项在接入时验证三平台构建）
- 日期：2026-09-30

## 背景

现有 Nagram 代码中有几处自研或平台专属实现，已有成熟、广泛采用的替代品。项目没有历史负担，按原则 5 直接替换。

## 决定

| 能力 | 现状 | 替换为 | 理由 |
| --- | --- | --- | --- |
| 简繁转换 | macOS `CFStringTransform`、Windows `LCMapStringEx` 逐字转换；Linux 不可用 | OpenCC（Apache-2.0） | 词组级转换，三平台一致；手机版 Nagram 使用同一库；以子模块在源码树内构建，发行版打包可用 `SEREIN_USE_SYSTEM_OPENCC=ON` 改用系统 `opencc` |
| 凭据存储 | 手写 Keychain 与 Windows 凭据管理器调用；Linux 不可用 | QtKeychain（BSD-3-Clause） | 覆盖 macOS Keychain、Windows 凭据管理器、Linux Secret Service；多数发行版有 `qt6keychain` 包 |
| 单元测试 | 手写 `int main()` 测试框架 | Catch2 v3（BSL-1.0），仅测试目标使用 | 最广泛使用的 C++ 测试框架之一；上游 `lib_base` 的测试也采用 Catch 风格 |
| 代码生成与守卫的 Python 依赖 | 无 | uv 锁定 `protobuf`、`jinja2` | 可复现；只在开发与 CI 的生成、检查步骤使用，不进入产品构建 |

保留且不替换：Qt Network（HTTP）、Qt Core JSON（生成的编解码基于它）、`QRegularExpression`（PCRE2，已有匹配预算保护）。

不采用：nlohmann/json（Qt Core JSON 已满足）、sqlite_orm（Qt SQL 已满足）、把第三方单头文件直接拷进源码树（无法随上游更新）。

## 验证门槛

- OpenCC：按 ADR-0005 以文本词典随资源分发、首次使用时解压，不在构建时生成；`test_serein` 覆盖简繁双向与词组样例。
- QtKeychain：Linux Docker 镜像需要 libsecret 开发包；不可用时在该平台显示“不可用”并保持现有行为，同时记录到本 ADR。
- Catch2：以子模块加入，只链接到测试目标。

## 修订（2026-09-30）：凭据存储不引入 QtKeychain

验证时发现上游的 Rocky Linux 8 构建镜像没有 libsecret 开发包。QtKeychain 在 Linux 上失去 libsecret 后只剩 KWallet 后端，GNOME 等桌面仍然不可用；补 libsecret 要改上游 Dockerfile，增加上游侵入。自行用 `generate_dbus` 实现 Secret Service 客户端也不合适：钥匙环加锁时需要等待异步的 Prompt 信号，而凭据接口是同步的，在主线程等待用户输入钥匙环密码会阻塞界面。

决定：

- macOS 与 Windows 继续使用系统凭据库（Keychain、Windows 凭据管理器），实现为 `serein/adapters/credentials/` 下按平台由 CMake 选择的 `keychain.cpp` 与 `wincred.cpp`。
- 其他平台改用本地加密存储 `tdata/serein_credentials`（`adapters/credentials/local_file.cpp`）：每个凭据用当前账号的本地密钥（新式 tdata 中所有账号共用的域级密钥，设置本地密码时由密码派生）派生的 AES-256-GCM 密钥单独加密（派生方式与历史库相同：对上下文 `serein-credentials-v1`、分隔字节与本地密钥做 SHA-256，本地密钥为 256 字节随机数），整体以 `QSaveFile` 原子写入。这与 Telegram 保存自身授权密钥的方式一致，凭据仍不进偏好、不随设置导出，也不依赖桌面环境。
- 未登录任何账号时读取与写入返回“不可用”；清除凭据时文件中不再有条目即删除文件。

Catch2 暂缓：现有测试以 `Require` 断言与单一入口组织，已覆盖全部纯逻辑模块并在三平台 CI 运行；迁移收益不足以抵消把几十个测试文件改写成 Catch2 用例的成本，待测试规模增长后再评估。
