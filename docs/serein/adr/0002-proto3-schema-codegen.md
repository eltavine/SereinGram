# ADR-0002：用 proto3 声明设置与配置

- 状态：Accepted
- 日期：2026-09-30

## 背景

需要一个声明来源同时驱动：设置注册表、设置页与搜索、默认值与校验、配置导出导入、结构化配置（过滤、链接、服务、菜单、排序）和历史记录载荷；格式必须能增量演进并在 CI 中检测不兼容变更。当前实现是手写的 `Option<T>` 注册表、24 个手写设置页文件，以及每种结构化配置各自手写的 JSON 解析。

约束（均已在仓库中核对）：

- 上游不带 protobuf 运行时：cld3 用手写头文件替代生成代码（`cmake/external/cld3/CMakeLists.txt`），WebRTC 以 `WEBRTC_ENABLE_PROTOBUF=0` 构建。
- WebRTC 自带的 abseil 在其使用方的头文件搜索路径上（`cmake/external/webrtc/CMakeLists.txt`）；Google protobuf 需要另一份 abseil，同时出现在 `Telegram` 目标中有头文件遮蔽与重复符号的风险。
- Windows 与 macOS 依赖由上游 `prepare.py` 静态构建（Windows 使用静态 MSVC 运行时），Linux 使用上游 Docker 镜像；任何新运行时库都要改这三处上游构建脚本，发行版打包也要多一个依赖。
- 静态 Qt 只包含 `qtbase`、`qtimageformats`、`qtshadertools`、`qtsvg`，没有 Qt Protobuf 所在的 `qtgrpc`。

## 方案比较

| 方案 | 优点 | 缺点 |
| --- | --- | --- |
| A. Google protobuf C++ 运行时 | 最成熟；运行时反射可读取自定义选项；标准 JSON 映射 | 新增 protobuf 与 abseil 两个重依赖；与 WebRTC 的 abseil 冲突；改动三处上游构建脚本 |
| B. Qt Protobuf（qtgrpc） | Qt 官方；生成 Qt 类型；有 JSON 序列化 | 需在 Qt 构建中加入 qtgrpc，生成器还需主机端 libprotoc；运行时读不到自定义选项，设置页元数据仍需另一套生成 |
| C. FlatBuffers | 头文件运行时；自带演进检查 | 不是 proto3，Buf 不支持；与项目采用的 Buf 工具链不一致 |
| D. proto3 + Buf + protovalidate 注解 + 本地 protoc 插件生成 Qt C++，无运行时库 | 零新增运行时依赖；生成代码的形状贴合项目（Qt 类型、`rpl`、上游偏好 KV）；Buf 负责 lint、breaking 与生成编排 | 需要维护生成器与 JSON 编解码模板 |

## 决定

采用 D：

- schema：`proto/serein/**`，Buf v2 配置；lint 用 `STANDARD`，breaking 检查 `FILE` 与 `WIRE_JSON`。
- 校验：protovalidate 标准注解，生成器只接受范围、`in` 列表、长度与正则约束，其余报错（fail closed）。默认值等价于“未设置”，生成的校验总是接受默认值；因此 `optional` 字段不使用 `IGNORE_IF_ZERO_VALUE`（`buf lint` 也要求如此）。
- 生成器：`tools/serein/codegen`。`buf build` 输出 JSON 描述符映像，其中自定义选项与 protovalidate 注解已由 Buf 解析；Python + Jinja2 渲染模板，依赖以 PEP 723 内联元数据声明并由 uv 运行。不需要 Python protobuf 库或 protoc 插件协议；上游 Docker 生成脚本本身也使用 Jinja2。
- 第一阶段生成与现有 `Option<T>` 同形的声明和 `RegisterOptions`，调用方无需改动；迁移时用新旧声明逐项比较（元数据、注册表顺序、校验在全部样本上的结果）证明等价。
- 第二阶段为带 `(serein.options.v1.file)` 选项的文件生成值类型与 JSON 编解码，带 `(serein.options.v1.document)` 的消息另有带版本号的 `Parse…`/`Serialize…`。映射遵循 proto3 标准 JSON 规则的子集：bool、int32、int64（写为字符串，读时也接受整数）、字符串、bytes（base64）、枚举（全名，读时也接受数值）、repeated、同文件内的嵌套消息、`optional` 存在性。所有非 `optional` 字段都会写出，缺失字段取默认值；未知字段严格拒绝，与现有配置格式的严格校验一致。map、oneof 与跨文件类型暂不支持，遇到即报错。
- 生成代码提交入库，CI 校验无漂移；构建机与发行版打包不需要 Buf 或 Python 依赖。

## 后果

- 手写注册表、通用设置页和各结构化配置的 JSON 解析由生成代码替代。
- 生成器按模块拆分，单文件不超过 1000 行，并用 golden 文件测试。
- 退出路径：生成的 API 与 proto3 语义一一对应；若上游以后带入 Qt Protobuf 或 Google protobuf，只需替换生成器与存储适配器，schema 与功能代码不变。
