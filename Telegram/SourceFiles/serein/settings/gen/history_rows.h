// Generated from proto/serein/settings/v1/history.proto by tools/serein/codegen; do not edit.
#pragma once

#include "base/basic_types.h"
#include "lang/lang_keys.h"
#include "serein/schema/gen/settings/history.h"
#include "serein/settings/rows.h"
#include "styles/style_menu_icons.h"

#include <array>

namespace Serein::HistorySettings {

inline const auto kToggleRows = std::array<ToggleRow, 6>{ {
	{
		&kHistorySaveDeleted,
		tr::lng_serein_history_save_deleted,
		u"serein/history/history-save-deleted"_q,
		{ u"deleted"_q, u"anti-recall"_q, u"history"_q },
	},
	{
		&kHistorySaveEdits,
		tr::lng_serein_history_save_edits,
		u"serein/history/history-save-edits"_q,
		{ u"edit"_q, u"history"_q },
	},
	{
		&kHistoryIncludeBots,
		tr::lng_serein_history_include_bots,
		u"serein/history/history-include-bots"_q,
		{ u"bots"_q, u"history"_q },
	},
	{
		&kHistoryKeepDeletedInPlace,
		tr::lng_serein_history_keep_deleted_in_place,
		u"serein/history/history-keep-deleted-in-place"_q,
		{ u"deleted"_q, u"anti-recall"_q, u"in chat"_q },
	},
	{
		&kHistoryKeepExpiredMedia,
		tr::lng_serein_history_keep_expired_media,
		u"serein/history/history-keep-expired-media"_q,
		{ u"self-destruct"_q, u"view once"_q, u"expired"_q, u"media"_q },
	},
	{
		&kHistoryKeepRemovedChats,
		tr::lng_serein_history_keep_removed_chats,
		u"serein/history/history-keep-removed-chats"_q,
		{ u"banned"_q, u"kicked"_q, u"removed"_q, u"channel"_q, u"group"_q },
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
	});
	AddNumber(builder, {
		.option = &kHistoryMaxRecords,
		.title = tr::lng_serein_history_max_records,
		.id = u"serein/history/history-max-records"_q,
		.keywords = { u"history"_q, u"limit"_q, u"records"_q },
		.minimum = 1,
		.maximum = 10000000,
		.zeroLabel = tr::lng_serein_history_unlimited,
	});
	EndSection(builder, tr::lng_serein_history_note);
}

inline constexpr auto kSubpageTitle = &tr::lng_serein_history_page;
inline const auto kSubpageIcon = &st::menuIconRestore;

inline void AddSubpageButton(
		::Settings::Builder::SectionBuilder &builder,
		::Settings::Type section) {
	builder.addSectionButton({
		.title = (*kSubpageTitle)(),
		.targetSection = section,
		.icon = { kSubpageIcon },
		.keywords = { u"deleted"_q, u"edited"_q, u"anti-recall"_q, u"history"_q },
	});
}

} // namespace Serein::HistorySettings
