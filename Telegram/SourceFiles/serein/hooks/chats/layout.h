#pragma once

namespace style {
struct DialogRow;
} // namespace style

namespace Serein::Chats {

[[nodiscard]] const style::DialogRow &RowStyle(bool hasTags, bool wideRow);
[[nodiscard]] int PreviewLines();
[[nodiscard]] bool HideSavedAndArchivedPreviews();
[[nodiscard]] bool HidePreview(bool folder, bool savedMessages);
[[nodiscard]] bool HideStories();
[[nodiscard]] bool ShowArchiveInFolders();
[[nodiscard]] bool HideFolderUnreadCounters();

} // namespace Serein::Chats
