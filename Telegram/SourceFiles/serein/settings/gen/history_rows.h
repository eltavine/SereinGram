// Generated from proto/serein/settings/v1/history.proto by tools/serein/codegen; do not edit.
#pragma once

#include "base/basic_types.h"
#include "lang/lang_keys.h"
#include "serein/schema/gen/settings/history.h"
#include "serein/settings/rows.h"

#include <array>

namespace Serein::HistorySettings {

inline const auto kToggleRows = std::array<ToggleRow, 4>{ {
	{
		&kHistorySaveDeleted,
		tr::lng_serein_history_save_deleted,
		u"serein/history/history-save-deleted"_q,
		{ u"deleted"_q, u"anti-recall"_q, u"history"_q },
	},
	{
		&kHistoryKeepDeletedInPlace,
		tr::lng_serein_history_keep_deleted_in_place,
		u"serein/history/history-keep-deleted-in-place"_q,
		{ u"deleted"_q, u"anti-recall"_q, u"in chat"_q },
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
} };

struct CustomRows {
	CustomRow historyRetentionDays;
	CustomRow historyMaxRecords;
};

inline void AddLayout(
		::Settings::Builder::SectionBuilder &builder,
		const CustomRows &custom) {
	AddToggle(builder, kToggleRows[0]);
	AddToggle(builder, kToggleRows[1]);
	AddToggle(builder, kToggleRows[2]);
	AddToggle(builder, kToggleRows[3]);
	custom.historyRetentionDays();
	custom.historyMaxRecords();
	AddNote(builder, tr::lng_serein_history_note);
}

} // namespace Serein::HistorySettings
