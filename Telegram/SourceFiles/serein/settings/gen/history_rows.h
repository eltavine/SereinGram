// Generated from proto/serein/settings/v1/history.proto by tools/serein/codegen; do not edit.
#pragma once

#include "base/basic_types.h"
#include "lang/lang_keys.h"
#include "serein/schema/gen/settings/history.h"
#include "serein/settings/rows.h"

#include <array>

namespace Serein::HistorySettings {

inline const auto kToggleRows = std::array<ToggleRow, 3>{ {
	{
		&kHistorySaveDeleted,
		tr::lng_serein_history_save_deleted,
		u"serein/history/history-save-deleted"_q,
		{  },
	},
	{
		&kHistorySaveEdits,
		tr::lng_serein_history_save_edits,
		u"serein/history/history-save-edits"_q,
		{  },
	},
	{
		&kHistoryIncludeBots,
		tr::lng_serein_history_include_bots,
		u"serein/history/history-include-bots"_q,
		{  },
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
	custom.historyRetentionDays();
	custom.historyMaxRecords();
}

} // namespace Serein::HistorySettings
