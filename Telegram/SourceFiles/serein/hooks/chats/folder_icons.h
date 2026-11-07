#pragma once

#include <QtCore/QString>

#include <optional>
#include <span>
#include <vector>

namespace Ui {
enum class FilterIcon : uchar;
struct FilterIcons;
} // namespace Ui

namespace Serein::Chats {

[[nodiscard]] const Ui::FilterIcons *ExtraFolderIcon(Ui::FilterIcon icon);
[[nodiscard]] std::optional<Ui::FilterIcon> FindExtraFolderIcon(const QString &emoji);
[[nodiscard]] std::vector<Ui::FilterIcon> FolderIconChoices(
	std::span<const Ui::FilterIcon> builtins);

} // namespace Serein::Chats
