# ADR-0004：用成熟依赖替换自研实现

- 状态：Accepted（各项在接入时验证三平台构建）
- 日期：2026-09-30

## 背景

现有 Nagram 代码中有几处自研或平台专属实现，已有成熟、广泛采用的替代品。项目没有历史负担，按原则 5 直接替换。

## 决定

| 能力 | 现状 | 替换为 | 理由 |
| --- | --- | --- | --- |
| 简繁转换 | macOS `CFStringTransform`、Windows `LCMapStringEx` 逐字转换；Linux 不可用 | OpenCC（Apache-2.0） | 词组级转换，三平台一致；手机版 Nagram 使用同一库；以子模块在源码树内构建，发行版打包使用系统 `opencc` |
| 凭据存储 | 手写 Keychain 与 Windows 凭据管理器调用；Linux 不可用 | QtKeychain（BSD-3-Clause） | 覆盖 macOS Keychain、Windows 凭据管理器、Linux Secret Service；多数发行版有 `qt6keychain` 包 |
| 单元测试 | 手写 `int main()` 测试框架 | Catch2 v3（BSL-1.0），仅测试目标使用 | 最广泛使用的 C++ 测试框架之一；上游 `lib_base` 的测试也采用 Catch 风格 |
| 代码生成与守卫的 Python 依赖 | 无 | uv 锁定 `protobuf`、`jinja2` | 可复现；只在开发与 CI 的生成、检查步骤使用，不进入产品构建 |

保留且不替换：Qt Network（HTTP）、Qt Core JSON（生成的编解码基于它）、`QRegularExpression`（PCRE2，已有匹配预算保护）。

不采用：nlohmann/json（Qt Core JSON 已满足）、sqlite_orm（Qt SQL 已满足）、把第三方单头文件直接拷进源码树（无法随上游更新）。

## 验证门槛

- OpenCC：词典在构建时生成；三平台 CI 构建通过，`test_serein` 覆盖简繁双向与词组样例。
- QtKeychain：Linux Docker 镜像需要 libsecret 开发包；不可用时在该平台显示“不可用”并保持现有行为，同时记录到本 ADR。
- Catch2：以子模块加入，只链接到测试目标。
