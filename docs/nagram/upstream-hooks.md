# 上游处理点

本文件列出 [设置页设计](settings-page.md) 中每个条目需要改动的上游逻辑，路径相对 `Telegram/SourceFiles/`。位置来自旧实现（`main` 分支）的实际调用点，已核对这些文件在当前上游 `dev` 中仍然存在；具体函数在实现该步骤时以当时的上游代码为准重新确认。

旧实现只作为“在哪里改”的线索，不要求复用其代码（见 [分步实施计划](implementation-plan.md) 第 1 节）。

## 1. 共用机制

以下机制由基础步骤提供，各条目只调用，不各自实现。

| 机制 | 做法 | 替代旧实现的做法 |
| --- | --- | --- |
| 选项读取 | 本机经 `Nagram::ForDevice()`、账号经 `Nagram::ForAccount(session)` 获取共享 `Options`，再调用 `Get` / `Value` | 旧实现的 `Nagram::Option` 大枚举 |
| 消息视图刷新 | `nagram/display/` 中的 `ViewRefresher`：随每个 `Main::Session` 订阅“消息显示类”选项，变化时对已加载消息调用上游 `Data::Session::requestItemViewRefresh` / `requestItemResize`。构造时一处调用，`Data::Session` 另加一处 `friend` 标记以访问已加载消息 | 旧实现在 `data/data_session.cpp` 中写了 31 处订阅与刷新逻辑 |
| 会话列表刷新 | `ListRefresher` 订阅注册表的 `RefreshDialogList` 标记，调用列表的行高重算与重绘接口；`Dialogs::InnerWidget` 构造时接入一处，头文件加一处 `friend` | 旧实现分散在 `dialogs/` 各文件 |
| 输入区刷新 | `nagram/compose/` 提供 `ButtonsChanged`；`HistoryWidget` 的短订阅块调用其私有刷新方法，`ComposeControls` 订阅同一事件 | 旧实现两处各自读取 11 个开关 |
| 消息菜单 | `Nagram::Menu::Tag` 标记 + `Nagram::Menu::Apply` 后处理（见设计文档第 3.4 节） | 旧实现在两个菜单文件中插入约 270 处调用 |
| 文案 | 英文在 `Resources/langs/nagram/nagram.strings`；简繁通过 `lang/lang_instance.cpp` 的一个挂钩作为缺失键的后备值 | 旧实现修改上游 `lang.strings` 与 `lang_instance.cpp` 81 行 |
| 设置入口 | `settings/sections/settings_main.cpp` 的 `BuildSectionButtons` 第一项加入 Nagram 分栏按钮；Nagram 页面本身在 `nagram/settings/` | 旧实现在同一文件中加入入口 |
| 存储 | 本机：`Core::Settings::readPref/writePref`；账号：`Storage::Account::readPref/writePref` | 旧实现向 `Main::SessionSettings` 二进制流尾部追加字段 |

消息视图由两套实现渲染：`history/history_inner_widget.cpp`（普通聊天）和 `history/view/history_view_list_widget.cpp`（话题、计划消息等）。所有显示条目必须两处都生效，验收时分别检查。

`nagram/core/options.cpp` 为 `Storage::Account` 提供了 `QByteArray` 偏好读写特化。同步上游时，若上游也增加同名特化，需检查并移除重复定义；重复符号会使链接失败。

## 2. 各条目

“方式”列：**读取**＝在上游判断处读取选项；**替换**＝用 Nagram 函数返回的值替换上游常量或计算结果；**过滤**＝在上游生成列表后移除项目；**拦截**＝在上游动作执行前插入确认或改道。

### 2.1 界面

| 编号 | 上游位置 | 需要处理的上游逻辑 | 方式 |
| --- | --- | --- | --- |
| A02 | `ui/chat/chat_style_radius.cpp`、`core/application.cpp` | 气泡圆角半径由常量改为按比例计算，启动时设定一次 | 替换 |
| A03、A04 | `ui/userpic_view.cpp`、`ui/controls/userpic_button.cpp`、`ui/peer/video_userpic_player.cpp` | 圆形头像的绘制路径改为圆角矩形；论坛和频道私信的特殊形状按 A04 决定是否统一 | 替换 |
| A05、A06 | `history/view/history_view_message.cpp`（最大气泡宽度计算） | 纯文字消息的最大宽度乘以比例；频道文字消息使用可用宽度；两者同时设置时 A05 优先 | 替换 |
| A07 | `history/view/history_view_message.cpp`（气泡绘制） | 不绘制尾巴，保留相连消息的圆角 | 读取 |
| A08 | `history/view/history_view_reply.cpp`、`ui/chat/chat_style.cpp`、`media/stories/media_stories_repost_view.cpp` | 回复与引用块使用主题色，不使用发送者自定义颜色与背景图案 | 读取 |
| A09 | `history/view/history_view_reply.cpp` | 不加载、不绘制回复缩略图，并回收其宽度 | 读取 |
| A10 | `window/section_widget.cpp`（对话主题与壁纸解析） | 解析对话主题时返回空，使用全局主题 | 读取 |
| A11 | `window/window_main_menu.cpp`（`setupMenu`） | 标题替换账号名；菜单项按配置重排与隐藏；节日装饰条件（`CheckSpecialEvent`）增加开关 | 过滤 |
| A12 | `main/main_domain.cpp`（未读角标计数）、`core/application.cpp` | 角标计数返回 0，托盘和窗口标题计数不变 | 替换 |
| A13、A14 | `window/notifications_manager.cpp`（通知调度等待时间） | 等待时间改为配置值，保留合并消息所需的最短等待 | 替换 |
| A15 | `lang/lang_instance.cpp`（取值出口） | 取出界面字符串时把全角 ASCII 与标点投影为半角 | 替换 |

### 2.2 聊天列表

| 编号 | 上游位置 | 需要处理的上游逻辑 | 方式 |
| --- | --- | --- | --- |
| B01 | `dialogs/dialogs_row.cpp`、`dialogs/dialogs_inner_widget.cpp`、`dialogs/dialogs.style` | 普通会话行使用紧凑行高与头像尺寸；更新列表高度与命中区域 | 替换 |
| B02 | `dialogs/ui/dialogs_layout.cpp`、`dialogs/dialogs_row.cpp` | 预览文字的最大行数与行高 | 替换 |
| B03 | `dialogs/ui/dialogs_layout.cpp`、`dialogs/dialogs_inner_widget_accessibility.cpp` | 收藏夹与归档行不绘制预览文字，读屏文本同步脱敏 | 读取 |
| B04 | `dialogs/dialogs_widget.cpp` | 即时隐藏动态条并收起已展开区域，保留内部对象（用户在 S30 确认沿用旧版行为） | 读取 |
| B05 | `window/window_session_controller.cpp`（初始文件夹）、`data/data_chat_filters.cpp` | 账号启动时选择文件夹；记录上次打开的文件夹 | 替换 |
| B06 | `data/data_chat_filters.cpp`、`ui/widgets/chat_filters_tabs_strip.cpp`、`window/window_filters_menu.cpp` | 至少有一个可用的其他文件夹时，从显示列表去掉“全部会话”；保存排序时保持其原位置 | 过滤 |
| B07 | `dialogs/dialogs_inner_widget.cpp` | 自定义文件夹列表顶部加入归档入口行 | 读取 |
| B08 | `ui/widgets/chat_filters_tabs_strip.cpp`、`window/window_filters_menu.cpp` | 不绘制文件夹未读数，读屏文本同步 | 读取 |
| B09 | `dialogs/dialogs_entry.cpp`、`dialogs/dialogs_list.cpp` | 会话排序键加入 Nagram 优先级（置顶之后、时间之前）；状态变化时更新该会话的位置 | 替换 |
| B10 | `data/components/sponsored_messages.cpp`、`dialogs/dialogs_inner_widget.cpp` | 不请求、注入赞助消息，切换时清除已显示的赞助消息；过滤搜索结果中的广告 | 读取 |
| B11 | `data/components/promo_suggestions.cpp` | 忽略代理赞助频道 | 读取 |
| B12、B13 | `dialogs/dialogs_top_bar_suggestion.cpp` | 顶部提示条不显示 Premium 推广与生日提示 | 过滤 |
| B14、B15 | `history/history_view_pull_to_next_channel.cpp` | 滚动到底时不触发切换，取消已排队的切换 | 读取 |
| 管理文件夹 | `data/data_chat_filters.cpp`、`ui/widgets/chat_filters_tabs_strip.cpp`、`window/window_filters_menu.cpp` | 文件夹匹配增加“仅我管理的”条件；文件夹菜单加入该选项 | 读取 |

### 2.3 消息

| 编号 | 上游位置 | 需要处理的上游逻辑 | 方式 |
| --- | --- | --- | --- |
| C01 | `history/view/history_view_bottom_info.cpp`、`history/view/history_view_element.cpp` | 时间格式加入秒 | 替换 |
| C02 | `history/view/history_view_bottom_info.cpp` | 转发消息显示 `originalDate` | 替换 |
| C03 | `history/view/history_view_element.cpp`（服务消息） | 服务消息文本后追加时间 | 读取 |
| C04 | `history/view/history_view_element.cpp`（时间提示） | 提示文本追加服务端消息 ID；本地、待发送消息不显示 | 读取 |
| C05–C07 | `history/view/history_view_bottom_info.cpp` | 计数格式化、浏览数与签名的布局 | 替换 |
| C08、C09 | `history/view/history_view_bottom_info.cpp` | “已编辑”标记的显示与文字 | 替换 |
| C10–C13 | `history/view/history_view_element.cpp`、`history/view/history_view_message.cpp` | 反应区域不创建并回收空间；按对话类型判断 | 读取 |
| C14 | `history/view/reactions/history_view_reactions_selector.cpp` | 右键菜单不附加反应面板 | 读取 |
| C15 | `history/history_inner_widget.cpp`、`history/view/history_view_list_widget.cpp` | 有选中消息时不附加反应面板 | 读取 |
| C16 | `history/view/media/history_view_sticker.cpp`、`history/view/history_view_emoji_interactions.cpp`、`history/view/history_view_emoji_interactions.h` | 不播放 Premium 贴纸外围特效；头文件以 `friend` 标记让 `nagram/messages/effects.cpp` 在开关变化时清理进行中的特效 | 读取 |
| C17 | `history/view/history_view_emoji_interactions.cpp` | 丢弃收到和本地触发的表情互动 | 读取 |
| C18 | `history/view/history_view_emoji_interactions.cpp` | 不播放消息附带特效，保留元数据 | 读取 |
| C19 | `history/view/history_view_element.cpp`、`history/view/history_view_text_helper.cpp`、`history/view/media/history_view_media.cpp` | 文字剧透与普通图片/视频剧透默认展开 | 读取 |
| C20 | `history/view/history_view_message.cpp` | 不显示快速转发按钮及其命中区域 | 读取 |
| C21 | `history/view/history_view_element.cpp` | 不插入推荐频道卡片 | 读取 |
| C22 | `info/profile/info_profile_badge.cpp`、`dialogs/dialogs_inner_widget_accessibility.cpp`、`dialogs/dialogs_inner_widget.cpp`、`main/main_session.cpp` | 不绘制会员星标与表情状态，认证和警告标识保留；列表订阅开关变化后重绘 | 读取 |
| C23 | `dialogs/dialogs_search_tags.cpp`、`history/view/reactions/history_view_reactions_selector.cpp` | 不显示未选中的收藏标签与标签选择器 | 过滤 |
| C24 | `history/view/history_view_send_action.cpp`、`history/view/history_view_top_bar_widget.cpp`、`dialogs/dialogs_inner_widget.cpp` | 私聊中不显示对方的输入、录制等状态 | 读取 |
| C25、C26 | `history/history_item.cpp`、`history/view/history_view_element.cpp` | 显示文本经过投影（间距、简繁），原文不变；投影结果按消息缓存 | 替换 |

### 2.4 输入与发送

| 编号 | 上游位置 | 需要处理的上游逻辑 | 方式 |
| --- | --- | --- | --- |
| D01–D10 | `history/history_widget.cpp`、`history/view/controls/history_view_compose_controls.cpp` | 各按钮的可见性与布局宽度；隐藏录音按钮时空草稿显示发送按钮 | 读取 |
| D11 | `history/history_widget.cpp`、`history/view/controls/history_view_bottom_controls.cpp` | 频道底部静音按钮 | 读取 |
| D12 | `chat_helpers/tabbed_panel.cpp` | 悬停不触发打开，点击保留 | 读取 |
| D13 | `history/history_widget.cpp`、`history/view/controls/history_view_compose_controls.cpp` | 附件按钮不注册悬停菜单 | 读取 |
| D14 | `history/history_widget.cpp`、`history/view/history_view_chat_section.cpp`、`history/view/history_view_scheduled_section.cpp` | 命令链接点击改为插入输入框光标处 | 拦截 |
| D15 | `history/history_widget.cpp`、`history/view/controls/history_view_compose_controls.cpp` | 输入框占位文字 | 替换 |
| D16 | `chat_helpers/message_field.cpp` | 不做 Markdown 自动转换 | 读取 |
| D17 | `history/view/controls/history_view_webpage_processor.cpp` | 输入时不请求预览；发送时带无预览标志；手动选择的预览保留 | 读取 |
| D18、D19 | `api/api_sending.cpp`、`api/api_editing.cpp`、`apiwrap.cpp`、`data/components/ephemeral_messages.cpp` | 发送与编辑前对文本做间距处理，保持实体偏移 | 替换 |
| D20、D21 | `chat_helpers/message_field.cpp` | 代码块默认语言；输入框菜单加入快捷回复 | 读取 |
| D22、D23 | `history/history_widget.cpp`、`history/view/history_view_chat_section.cpp` | 发送贴纸 / GIF 前弹出确认，回调只执行一次 | 拦截 |
| D24、D25 | `history/view/controls/history_view_voice_record_bar.cpp` | 录制结束后进入上游的试听界面而不是直接发送 | 读取 |
| D26 | `calls/calls_instance.cpp` | 发起私聊通话前进入上游确认 | 拦截 |
| D27 | `apiwrap.cpp`、`boxes/share_box.cpp` | 转发与附言的发送顺序 | 替换 |

### 2.5 消息菜单

| 编号 | 上游位置 | 需要处理的上游逻辑 | 方式 |
| --- | --- | --- | --- |
| E01–E14 | `history/history_inner_widget.cpp`（`showContextMenu`）、`history/view/history_view_context_menu.cpp`（`FillContextMenu`） | 创建菜单项处加 `Tag`；填充结束处调用一次 `Apply` | 过滤 |
| E15–E23 | 同上（`Apply` 内插入） | Nagram 动作在 `nagram/menu/` 中实现，检查权限与消息有效性 | 读取 |
| E24 | `nagram/menu/` 内部 | 复读前确认 | — |
| 上游其他菜单 | `window/window_peer_menu.cpp`（G08、本地别名入口） | 会话菜单项过滤与新增 | 过滤 |

### 2.6 媒体与贴纸

| 编号 | 上游位置 | 需要处理的上游逻辑 | 方式 |
| --- | --- | --- | --- |
| F01 | `history/view/media/history_view_sticker.cpp` | 贴纸显示尺寸乘以比例；表情与骰子保持原尺寸 | 替换 |
| F02 | `history/view/history_view_bottom_info.cpp`、`.h` | 贴纸隐藏时间，保留发送状态 | 读取 |
| F03 | `chat_helpers/stickers_list_widget.cpp` | 最近贴纸显示数量 | 替换 |
| F04、F05 | `chat_helpers/stickers_list_widget.cpp` | 不显示群组贴纸区与推荐贴纸 | 过滤 |
| F06 | `chat_helpers/emoji_list_widget.cpp` | 不显示推荐表情 | 过滤 |
| F07 | `chat_helpers/stickers_list_footer.cpp` | 不显示 GIF 推荐分类 | 过滤 |
| F08 | `history/view/history_view_about_view.cpp` | 新私聊不显示问候贴纸 | 读取 |
| F09 | `history/view/media/history_view_gif.cpp` | 视频与圆形视频不自动播放 | 读取 |
| F10 | `media/view/media_view_overlay_widget.cpp` | GIF 使用视频播放控制 | 读取 |
| F11 | `storage/localimageloader.cpp` | 以文件发送的 MP4 附加视频属性与预览 | 读取 |
| F12、F13 | 无上游改动（使用 `Data::Stickers` 已有接口） | — | — |

### 2.7 隐私与资料

| 编号 | 上游位置 | 需要处理的上游逻辑 | 方式 |
| --- | --- | --- | --- |
| G01 | 无上游改动（直接读写 `Main::SessionSettings::phoneNumberHidden`） | — | — |
| G02 | `core/application.cpp`（窗口保护原因）、`dialogs/ui/dialogs_layout.cpp`、`dialogs/dialogs_inner_widget.cpp`、`window/notifications_manager.cpp`、`window/main_window.cpp`（标题） | 加入窗口捕获保护原因；遮盖列表身份、预览、标题与通知内容 | 读取 |
| G03 | `api/api_who_reacted.cpp` | 不显示已读时间提示 | 读取 |
| G04 | `history/view/history_view_contact_status.cpp` | 不显示分享手机号提示 | 读取 |
| G05、G06 | `info/profile/info_profile_actions.cpp` | 资料页增加 ID 与数据中心行，取值逻辑在 `nagram/privacy/profile.cpp` | 读取 |
| G07 | `info/profile/tabs/adapters/info_profile_tab_peer_lists.cpp`、`info/profile/info_profile_shared_media_classic.cpp`、`info/profile/info_profile_top_bar.cpp` | 不显示礼物标签、礼物区、礼物按钮与置顶礼物 | 读取 |
| G08 | `window/window_peer_menu.cpp` | 不显示创建待办入口 | 过滤 |
| 本地别名 | `data/data_peer.cpp`（显示名）、`history/history.cpp`、`info/profile/info_profile_values.cpp`、`window/window_peer_menu.cpp` | 显示名与本地搜索使用别名，原名保留 | 替换 |

### 2.8 翻译与 AI

| 编号 | 上游位置 | 需要处理的上游逻辑 | 方式 |
| --- | --- | --- | --- |
| H01 | `boxes/translate_box.cpp` | 翻译请求交给所选服务；失败时显示错误，不改用其他服务 | 替换 |
| H02 | `api/api_transcribes.cpp`、`history/view/history_view_transcribe_button.cpp`、`history/view/media/history_view_document.cpp` | 转写请求交给所选服务；结果在原位置显示 | 替换 |
| H03 | 无上游改动 | — | — |
| H04 | `boxes/compose_ai_box.cpp`、`ui/controls/compose_ai_button_factory.cpp` | 草稿 AI 入口改由系统模型处理 | 拦截 |
| 草稿翻译 | `chat_helpers/message_field.cpp` | 输入框菜单加入“翻译草稿” | 读取 |

### 2.9 规则

| 编号 | 上游位置 | 需要处理的上游逻辑 | 方式 |
| --- | --- | --- | --- |
| I01 | `history/history_item.cpp`、`history/view/history_view_element.cpp` | 消息显示文本经过过滤投影；整条隐藏的消息不创建视图 | 替换 |
| I02 | `core/ui_integration.cpp`（外部链接打开） | 打开链接前按规则改写并确认 | 拦截 |

### 2.10 配置管理

无上游改动。批量导入先完成校验，再在同一事件循环内逐键写入并统一通知（见设计文档第 3.3 节）。

## 3. 改动面预估

上表去重后共涉及 85 个上游文件（已逐个确认在当前上游中存在），与旧实现的文件数相当：这些功能本身就分布在这些位置。上游改动以 `#include`、已有判断中的条件及单行调用为主；调用上游类私有方法时允许约 10 行以内的短块，并在提交正文说明原因。每个里程碑统计上游新增行数，解释集中改动，不再要求每个文件只改一行。M2 的 152 行调用／条件主要分布在输入按钮的既有判断处；D14 命令草稿分支与按钮刷新订阅因调用 `HistoryWidget` 私有方法而保留在上游文件。热点文件及其承载的条目：

| 文件 | 条目数 |
| --- | --- |
| `history/history_widget.cpp` | D01–D15、D22、D23 等约 17 项 |
| `history/view/controls/history_view_compose_controls.cpp` | D01–D10、D13、D15 等约 13 项 |
| `history/view/history_view_element.cpp` | C03、C04、C10–C13、C19、C21、C25、C26、F02、I01 等约 13 项 |
| `history/view/history_view_bottom_info.cpp` | C01、C02、C05–C09、F02 |
| `history/history_inner_widget.cpp`、`history/view/history_view_context_menu.cpp` | 消息菜单（E01–E23） |
