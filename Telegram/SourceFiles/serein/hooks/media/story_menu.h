#pragma once

#include <memory>

namespace ChatHelpers {
class Show;
} // namespace ChatHelpers

namespace Data {
class Story;
} // namespace Data

namespace Ui::Menu {
struct MenuCallback;
} // namespace Ui::Menu

namespace Serein::Hooks::Media {

void FillStoryMenu(
	const Ui::Menu::MenuCallback &addAction,
	std::shared_ptr<ChatHelpers::Show> show,
	Data::Story *story);

} // namespace Serein::Hooks::Media
