# ADR-0005：简繁转换改用 OpenCC

> 状态：Accepted · 已实施（OpenCC ver.1.4.2）· 日期：2026-09-30 · 关联：SG-TRANS-04

## 背景

阅读时的简繁转换（`serein/messages/reading.cpp`）目前调用系统接口：macOS 用 `CFStringTransform`，Windows 用 `LCMapStringEx`。两者都只做逐字映射，“头发/頭髮”“后来/後來”这类一对多的词组会转错；其余平台（Linux 与 BSD）没有可用接口，设置项显示为不可用。

## 决定

以 [OpenCC](https://github.com/BYVoid/OpenCC)（Apache License 2.0，与 GPLv3 兼容）替换系统接口，三平台使用同一套词组级转换。

1. **来源**：以 git 子模块引入 `Telegram/ThirdParty/OpenCC`，固定到正式发布标签；打包构建（`DESKTOP_APP_USE_PACKAGED`）改用系统的 `opencc` pkg-config 包。
2. **构建**：不使用 OpenCC 顶层 CMake（其 `data` 目录在配置期强制要求 Python，并引入 CTest 与命令行工具），改由 `Telegram/cmake/serein_opencc.cmake` 直接把 `src` 与自带 marisa 的源文件编译为静态库 `Serein::OpenCC`，导出宏头文件用 CMake 的 `GenerateExportHeader` 生成；该目标关闭警告即错误，包含目录标为 SYSTEM。升级 OpenCC 时核对源文件列表。
3. **词典**：不在构建期运行 `opencc_dict` 生成 `.ocd2`，而是把 `STPhrases`、`STCharacters`、`TSPhrases`、`TSCharacters` 等文本词典随程序资源分发，配置使用 OpenCC 的 `text` 词典类型；首次使用时解压到本机数据目录并按版本号缓存。这样三平台不需要额外的构建期工具。
4. **接口**：`ConvertChinese` 的签名不变，实现改为按转换方向缓存 `opencc::SimpleConverter`；实体偏移沿用现有的逐段映射，词组替换导致长度变化时按段重新计算偏移并由单元测试覆盖。
5. **回退**：OpenCC 初始化失败（资源缺失、解压失败）时记录日志并返回 `std::nullopt`，界面保持原文，不回退到系统逐字转换，避免两套结果不一致。

## 影响

- `ChineseConversionAvailable()` 在三平台都返回 `true`，Linux 用户获得该功能。
- 发行包增加约 2 MB 词典资源与一个静态库；首次转换有一次性的解压与加载开销。
- 上游侵入：`.gitmodules` 增加 3 行，其余改动在 `serein/`、`Telegram/cmake/` 与自有资源文件。
- 许可证：发行包需附带 OpenCC（Apache-2.0）、marisa-trie（BSD-2-Clause 或 LGPL-2.1+）、darts-clone（BSD-2-Clause）与 RapidJSON（MIT）的许可证文本，由 SG-PLAT 打包步骤负责。
- 实体偏移由 `ConvertChineseRuns` 按实体与受保护区间的边界分段转换，核心测试覆盖词组变长、受保护文本、转换失败与越界实体，并用真实词典验证“头发→頭髮”“後來→后来”。
