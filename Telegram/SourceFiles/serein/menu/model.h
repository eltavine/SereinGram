#pragma once

#include "serein/core/options.h"

#include <array>

namespace Serein::Menu {

enum class ActionId : int {
	Reply = 1,
	Edit,
	Copy,
	CopyLink,
	Forward,
	Translate,
	Pin,
	Select,
	Statistics,
	Report,
	BlockSender,
	Image,
	Delete,
	StickerPack,
	Repeat,
	RepeatAsCopy,
	ForwardWithoutQuote,
	Batch,
	SelectSender,
	MediaInfo,
	Screenshot = 21,
	Reading = 22,
	FilterAuthor = 23,
};

enum class Visibility { Show, Hide, WithOption };

struct Entry {
	ActionId id;
	const char *titleKey;
};

inline constexpr auto kEntries = std::array<Entry, 23>({{
	{ ActionId::Reply, "lng_serein_menu_reply" },
	{ ActionId::Edit, "lng_serein_menu_edit" },
	{ ActionId::Copy, "lng_serein_menu_copy" },
	{ ActionId::CopyLink, "lng_serein_menu_copy_link" },
	{ ActionId::Forward, "lng_serein_menu_forward" },
	{ ActionId::Translate, "lng_serein_menu_translate" },
	{ ActionId::Pin, "lng_serein_menu_pin" },
	{ ActionId::Select, "lng_serein_menu_select" },
	{ ActionId::Statistics, "lng_serein_menu_statistics" },
	{ ActionId::Report, "lng_serein_menu_report" },
	{ ActionId::BlockSender, "lng_serein_menu_block_sender" },
	{ ActionId::Image, "lng_serein_menu_image" },
	{ ActionId::Delete, "lng_serein_menu_delete" },
	{ ActionId::StickerPack, "lng_serein_menu_sticker_pack" },
	{ ActionId::Repeat, "lng_serein_menu_repeat" },
	{ ActionId::RepeatAsCopy, "lng_serein_menu_repeat_as_copy" },
	{ ActionId::ForwardWithoutQuote, "lng_serein_menu_forward_without_quote" },
	{ ActionId::Batch, "lng_serein_menu_batch" },
	{ ActionId::SelectSender, "lng_serein_menu_select_sender" },
	{ ActionId::MediaInfo, "lng_serein_menu_media_info" },
	{ ActionId::Screenshot, "lng_serein_menu_screenshot" },
	{ ActionId::Reading, "lng_serein_menu_reading" },
	{ ActionId::FilterAuthor, "lng_serein_filter_author_hide" },
}});

[[nodiscard]] Visibility DefaultVisibility(ActionId id);
[[nodiscard]] bool ValidateConfig(const QByteArray &raw);
[[nodiscard]] Visibility ReadVisibility(const QByteArray &raw, ActionId id);
[[nodiscard]] QByteArray WriteVisibility(
	const QByteArray &raw,
	ActionId id,
	Visibility visibility);
[[nodiscard]] bool Visible(Visibility visibility, bool optionHeld);

inline const auto kMenuConfig = Option<QByteArray>{
	"serein.messageMenu", Scope::Device, QByteArray(),
	Category::Menu, "lng_serein_menu", static_cast<unsigned>(Flag::Exportable),
	ValidateConfig };
inline constexpr auto kConfirmRepeat = Option<bool>{
	"serein.confirmRepeat", Scope::Device, false,
	Category::Menu, "lng_serein_menu_confirm_repeat",
	static_cast<unsigned>(Flag::Exportable) };

inline void RegisterOptions(Registry &registry) {
	Expects(registry.Add(kMenuConfig));
	Expects(registry.Add(kConfirmRepeat));
}

} // namespace Serein::Menu
