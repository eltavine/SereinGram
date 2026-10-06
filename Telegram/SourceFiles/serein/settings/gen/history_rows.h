// Generated from proto/serein/settings/v1/history.proto by tools/serein/codegen; do not edit.
#pragma once

#include "base/basic_types.h"
#include "lang/lang_keys.h"
#include "serein/schema/gen/settings/history.h"
#include "serein/settings/rows.h"
#include "styles/style_menu_icons.h"
#include "styles/style_serein.h"

#include <array>

namespace Serein::HistorySettings {

inline const auto kToggleRows = std::array<ToggleRow, 6>{ {
	{
		.option = &kHistorySaveDeleted,
		.title = tr::lng_serein_history_save_deleted,
		.id = u"serein/history/history-save-deleted"_q,
		.keywords = { u"deleted"_q, u"anti-recall"_q, u"history"_q },
		.icon = &st::menuIconDelete,
		.tile = &st::settingsIconBg3,
		.about = tr::lng_serein_history_save_deleted_about,
	},
	{
		.option = &kHistorySaveEdits,
		.title = tr::lng_serein_history_save_edits,
		.id = u"serein/history/history-save-edits"_q,
		.keywords = { u"edit"_q, u"history"_q },
		.icon = &st::menuIconEdit,
		.tile = &st::settingsIconBg3,
		.about = tr::lng_serein_history_save_edits_about,
	},
	{
		.option = &kHistoryIncludeBots,
		.title = tr::lng_serein_history_include_bots,
		.id = u"serein/history/history-include-bots"_q,
		.keywords = { u"bots"_q, u"history"_q },
		.icon = &st::menuIconBot,
		.tile = &st::settingsIconBg3,
		.about = tr::lng_serein_history_include_bots_about,
	},
	{
		.option = &kHistoryKeepDeletedInPlace,
		.title = tr::lng_serein_history_keep_deleted_in_place,
		.id = u"serein/history/history-keep-deleted-in-place"_q,
		.keywords = { u"deleted"_q, u"anti-recall"_q, u"in chat"_q },
		.icon = &st::menuIconShowInChat,
		.tile = &st::settingsIconBg6,
		.about = tr::lng_serein_history_keep_deleted_in_place_about,
	},
	{
		.option = &kHistoryKeepExpiredMedia,
		.title = tr::lng_serein_history_keep_expired_media,
		.id = u"serein/history/history-keep-expired-media"_q,
		.keywords = { u"self-destruct"_q, u"view once"_q, u"expired"_q, u"media"_q },
		.icon = &st::menuIconTTL,
		.tile = &st::settingsIconBg6,
		.about = tr::lng_serein_history_keep_expired_media_about,
	},
	{
		.option = &kHistoryKeepRemovedChats,
		.title = tr::lng_serein_history_keep_removed_chats,
		.id = u"serein/history/history-keep-removed-chats"_q,
		.keywords = { u"banned"_q, u"kicked"_q, u"removed"_q, u"channel"_q, u"group"_q },
		.icon = &st::menuIconRemovedUsers,
		.tile = &st::settingsIconBg6,
		.about = tr::lng_serein_history_keep_removed_chats_about,
	},
} };

inline void AddLayout(
		::Settings::Builder::SectionBuilder &builder) {
	AddSection(builder, {
		u"serein/history/record"_q,
		tr::lng_serein_section_history_record,
		{ u"deleted"_q, u"edited"_q, u"bots"_q },
	});
	AddToggle(builder, kToggleRows[0]);
	AddToggle(builder, kToggleRows[1]);
	AddToggle(builder, kToggleRows[2]);
	EndSection(builder);
	AddSection(builder, {
		u"serein/history/deleted"_q,
		tr::lng_serein_section_history_deleted,
		{ u"deleted"_q, u"media"_q },
	});
	AddToggle(builder, kToggleRows[3]);
	AddToggle(builder, kToggleRows[4]);
	AddToggle(builder, kToggleRows[5]);
	AddNote(builder, tr::lng_serein_history_keep_removed_chats_note);
	EndSection(builder, tr::lng_serein_section_history_deleted_note);
	AddSection(builder, {
		u"serein/history/storage"_q,
		tr::lng_serein_section_history_storage,
		{ u"storage"_q, u"days"_q, u"limit"_q },
	});
	AddNumber(builder, {
		.option = &kHistoryRetentionDays,
		.title = tr::lng_serein_history_retention_days,
		.id = u"serein/history/history-retention-days"_q,
		.keywords = { u"history"_q, u"retention"_q, u"days"_q },
		.minimum = 1,
		.maximum = 3650,
		.zeroLabel = tr::lng_serein_history_keep_forever,
		.format = [](int value) {
			return tr::lng_days(tr::now, lt_count, value);
		},
		.icon = &st::menuIconHourglass,
		.tile = &st::settingsIconBg1,
		.about = tr::lng_serein_history_retention_days_about,
	});
	AddNumber(builder, {
		.option = &kHistoryMaxRecords,
		.title = tr::lng_serein_history_max_records,
		.id = u"serein/history/history-max-records"_q,
		.keywords = { u"history"_q, u"limit"_q, u"records"_q },
		.minimum = 1,
		.maximum = 10000000,
		.zeroLabel = tr::lng_serein_history_unlimited,
		.icon = &st::menuIconStorage,
		.tile = &st::settingsIconBg1,
		.about = tr::lng_serein_history_max_records_about,
	});
	EndSection(builder, tr::lng_serein_history_note);
}

inline constexpr auto kSubpageTitle = &tr::lng_serein_history_page;
inline constexpr auto kSubpageAbout = &tr::lng_serein_page_history_about;
inline const auto kSubpageIcon = &st::menuIconRestore;
inline const auto kSubpageTile = &st::settingsIconBg3;

inline void AddSubpageButton(
		::Settings::Builder::SectionBuilder &builder,
		::Settings::Type section) {
	AddPageButton(builder, {
		.title = (*kSubpageTitle)(),
		.section = section,
		.icon = kSubpageIcon,
		.tile = kSubpageTile,
		.keywords = { u"deleted"_q, u"edited"_q, u"anti-recall"_q, u"history"_q },
		.about = *kSubpageAbout,
	});
}

} // namespace Serein::HistorySettings
