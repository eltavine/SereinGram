# ADR-0006：字体选择与 Unicode 回退

- 状态：Accepted
- 日期：2026-11-07

## 决定

等宽字体由设备级选项保存，在字体缓存初始化前交给 lib_ui。
系统字体选择器提供预览与等宽字体筛选，恢复默认只清除字体选择。
编程连字使用 Qt 的 `liga`、`clig` 与 `calt` 特性独立控制，设置变更后重启生效。

Qt 继续负责字体匹配、脚本整形与系统字体回退。随应用注册 GNU Unifont
17.0.04 的基础和补充平面字体，供系统缺少相应字体时使用；不按单个字符维护替换表。
另注册 Noto Sans Bamum、Linear A 与 Tai Tham，覆盖报告中的三个脚本示例。
回退字体共约 11.2 MiB，采用 OFL-1.1，许可原文随资源与应用许可页分发。
字形采用点阵轮廓，罕见字符的形态与复杂文字的连写质量仍受字体和 Qt 整形支持限制；
这不意味着支持全部 Unicode 字符。移除回退字体只需移除资源与启动注册。

## 依据与复现

- [Qt 字体匹配与特性](https://doc.qt.io/qt-6/qfont.html)：保留字体合并，避免禁用必要的脚本整形。
- [Qt 字体注册](https://doc.qt.io/qt-6/qfontdatabase.html)：应用字体进入统一的字体数据库。
- [GNU Unifont](https://unifoundry.com/unifont/)：广泛覆盖 Unicode，双许可中的 OFL-1.1 适合随应用分发。
- 固定文件来自 [17.0.04 发布目录](https://unifoundry.com/pub/unifont/unifont-17.0.04/font-builds/)。
- `unifont.otf` SHA-256：`d1f664a9753b9c6b7ff357128749e32b5d3eee90c7c03618363fabd43a39b5b7`。
- `unifont_upper.otf` SHA-256：`adce94bc065675242e9d65d8cc4c250de9471041f0c0f22deb9d14086d9f59ea`。

Noto 字体固定到 `notofonts/noto-fonts` 的
`ffebf8c1ee449e544955a7e813c54f9b73848eac`；沿用其 OFL-1.1 许可。

lib_ui 的定制版本只增加启动时的等宽字体配置入口，并在删除菜单项后重排索引。
子模块固定到已发布的分支，后续同步需保留这两项改动，直到上游包含等价实现。
