#pragma once

#include "nagram/core/options.h"

#include <array>

namespace Nagram::Menu {

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
};

enum class Visibility { Show, Hide, WithOption };

struct Entry {
	ActionId id;
	const char *titleKey;
};

inline constexpr auto kEntries = std::array<Entry, 20>({{
	{ ActionId::Reply, "lng_nagram_menu_reply" },
	{ ActionId::Edit, "lng_nagram_menu_edit" },
	{ ActionId::Copy, "lng_nagram_menu_copy" },
	{ ActionId::CopyLink, "lng_nagram_menu_copy_link" },
	{ ActionId::Forward, "lng_nagram_menu_forward" },
	{ ActionId::Translate, "lng_nagram_menu_translate" },
	{ ActionId::Pin, "lng_nagram_menu_pin" },
	{ ActionId::Select, "lng_nagram_menu_select" },
	{ ActionId::Statistics, "lng_nagram_menu_statistics" },
	{ ActionId::Report, "lng_nagram_menu_report" },
	{ ActionId::BlockSender, "lng_nagram_menu_block_sender" },
	{ ActionId::Image, "lng_nagram_menu_image" },
	{ ActionId::Delete, "lng_nagram_menu_delete" },
	{ ActionId::StickerPack, "lng_nagram_menu_sticker_pack" },
	{ ActionId::Repeat, "lng_nagram_menu_repeat" },
	{ ActionId::RepeatAsCopy, "lng_nagram_menu_repeat_as_copy" },
	{ ActionId::ForwardWithoutQuote, "lng_nagram_menu_forward_without_quote" },
	{ ActionId::Batch, "lng_nagram_menu_batch" },
	{ ActionId::SelectSender, "lng_nagram_menu_select_sender" },
	{ ActionId::MediaInfo, "lng_nagram_menu_media_info" },
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
	"nagram.messageMenu", Scope::Device, QByteArray(),
	Category::Menu, "lng_nagram_menu", static_cast<unsigned>(Flag::Exportable),
	ValidateConfig };
inline constexpr auto kConfirmRepeat = Option<bool>{
	"nagram.confirmRepeat", Scope::Device, false,
	Category::Menu, "lng_nagram_menu_confirm_repeat",
	static_cast<unsigned>(Flag::Exportable) };

inline void RegisterOptions(Registry &registry) {
	Expects(registry.Add(kMenuConfig));
	Expects(registry.Add(kConfirmRepeat));
}

} // namespace Nagram::Menu
