#pragma once

#include "serein/core/options.h"
#include "serein/schema/gen/settings/menu.h"
#include "serein/hooks/menu/action_id.h"

#include <array>

namespace Serein::Menu {

enum class Visibility { Show, Hide, WithOption };

struct Entry {
	ActionId id;
	const char *titleKey;
};

inline constexpr auto kEntries = std::array<Entry, 34>({{
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
	{ ActionId::EditHistory, "lng_serein_menu_edit_history" },
	{ ActionId::DeletedMessages, "lng_serein_menu_deleted_messages" },
	{ ActionId::ReadUntilHere, "lng_serein_menu_read_until_here" },
	{ ActionId::HistoryExclusion, "lng_serein_menu_history_exclude" },
	{ ActionId::ButtonData, "lng_serein_menu_button_data" },
	{ ActionId::MessageDetails, "lng_serein_menu_details" },
	{ ActionId::SelectRange, "lng_serein_menu_select_range" },
	{ ActionId::BatchUnpin, "lng_serein_menu_unpin_selected" },
	{ ActionId::QuickRatingFirst, "lng_serein_menu_quick_rating_first" },
	{ ActionId::QuickRatingSecond, "lng_serein_menu_quick_rating_second" },
	{ ActionId::Reminder, "lng_serein_menu_reminder" },
}});

[[nodiscard]] Visibility DefaultVisibility(ActionId id);
[[nodiscard]] bool ValidateConfig(const QByteArray &raw);
[[nodiscard]] Visibility ReadVisibility(const QByteArray &raw, ActionId id);
[[nodiscard]] QByteArray WriteVisibility(
	const QByteArray &raw,
	ActionId id,
	Visibility visibility);
[[nodiscard]] bool Visible(Visibility visibility, bool optionHeld);

} // namespace Serein::Menu
