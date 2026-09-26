#pragma once

namespace style {
struct DialogRow;
} // namespace style

namespace Nagram::Chats {

[[nodiscard]] const style::DialogRow &RowStyle(bool hasTags, bool wideRow);
[[nodiscard]] int PreviewLines();
[[nodiscard]] bool HideSavedAndArchivedPreviews();
[[nodiscard]] bool HidePreview(bool folder, bool savedMessages);
[[nodiscard]] bool HideStories();

} // namespace Nagram::Chats
