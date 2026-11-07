#include "serein/hooks/chats/folder_icons.h"

#include "serein/chats/options.h"
#include "ui/emoji_config.h"
#include "ui/filter_icons.h"

#include <array>

#include "styles/style_serein.h"

namespace Serein::Chats {
namespace {

constexpr auto kPackSize = 6;
constexpr auto kFirst = int(Ui::FilterIcon::SereinDesk);

[[nodiscard]] const auto &Icons() {
	static const auto result = std::array{
		Ui::FilterIcons{
			&st::sereinFolderDesk, &st::sereinFolderDeskActive,
			&st::sereinFolderDeskTabs, &st::sereinFolderDeskUserpic,
			u"🖥️"_q,
		},
		Ui::FilterIcons{
			&st::sereinFolderCalendar, &st::sereinFolderCalendarActive,
			&st::sereinFolderCalendarTabs, &st::sereinFolderCalendarUserpic,
			u"📅"_q,
		},
		Ui::FilterIcons{
			&st::sereinFolderPin, &st::sereinFolderPinActive,
			&st::sereinFolderPinTabs, &st::sereinFolderPinUserpic,
			u"📌"_q,
		},
		Ui::FilterIcons{
			&st::sereinFolderDocument, &st::sereinFolderDocumentActive,
			&st::sereinFolderDocumentTabs, &st::sereinFolderDocumentUserpic,
			u"📄"_q,
		},
		Ui::FilterIcons{
			&st::sereinFolderTools, &st::sereinFolderToolsActive,
			&st::sereinFolderToolsTabs, &st::sereinFolderToolsUserpic,
			u"🛠️"_q,
		},
		Ui::FilterIcons{
			&st::sereinFolderWrench, &st::sereinFolderWrenchActive,
			&st::sereinFolderWrenchTabs, &st::sereinFolderWrenchUserpic,
			u"🔧"_q,
		},
		Ui::FilterIcons{
			&st::sereinFolderCamera, &st::sereinFolderCameraActive,
			&st::sereinFolderCameraTabs, &st::sereinFolderCameraUserpic,
			u"📷"_q,
		},
		Ui::FilterIcons{
			&st::sereinFolderHeadphones, &st::sereinFolderHeadphonesActive,
			&st::sereinFolderHeadphonesTabs, &st::sereinFolderHeadphonesUserpic,
			u"🎧"_q,
		},
		Ui::FilterIcons{
			&st::sereinFolderPaw, &st::sereinFolderPawActive,
			&st::sereinFolderPawTabs, &st::sereinFolderPawUserpic,
			u"🐾"_q,
		},
		Ui::FilterIcons{
			&st::sereinFolderCoffee, &st::sereinFolderCoffeeActive,
			&st::sereinFolderCoffeeTabs, &st::sereinFolderCoffeeUserpic,
			u"☕"_q,
		},
		Ui::FilterIcons{
			&st::sereinFolderBooks, &st::sereinFolderBooksActive,
			&st::sereinFolderBooksTabs, &st::sereinFolderBooksUserpic,
			u"📚"_q,
		},
		Ui::FilterIcons{
			&st::sereinFolderMountain, &st::sereinFolderMountainActive,
			&st::sereinFolderMountainTabs, &st::sereinFolderMountainUserpic,
			u"🏔️"_q,
		},
	};
	return result;
}

} // namespace

const Ui::FilterIcons *ExtraFolderIcon(Ui::FilterIcon icon) {
	const auto index = int(icon) - kFirst;
	return (index >= 0 && index < int(Icons().size())) ? &Icons()[index] : nullptr;
}

std::optional<Ui::FilterIcon> FindExtraFolderIcon(const QString &emoji) {
	const auto parsed = Ui::Emoji::Find(emoji);
	for (auto i = 0; i != int(Icons().size()); ++i) {
		const auto &entry = Icons()[i];
		if (entry.emoji == emoji || (parsed && parsed == Ui::Emoji::Find(entry.emoji))) {
			return Ui::FilterIcon(kFirst + i);
		}
	}
	return std::nullopt;
}

std::vector<Ui::FilterIcon> FolderIconChoices(std::span<const Ui::FilterIcon> builtins) {
	auto result = std::vector<Ui::FilterIcon>(builtins.begin(), builtins.end());
	const auto pack = ForDevice().Get(kFolderIconPack);
	if (pack < 1 || pack > 2) return result;
	const auto offset = kFirst + (pack - 1) * kPackSize;
	for (auto i = 0; i != kPackSize; ++i) result.push_back(Ui::FilterIcon(offset + i));
	return result;
}

} // namespace Serein::Chats
