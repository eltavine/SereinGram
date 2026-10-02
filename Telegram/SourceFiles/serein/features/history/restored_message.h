#pragma once

#include "data/data_msg_id.h"
#include "history/history_item.h"
#include "serein/ports/history_store.h"

class History;

namespace Serein::HistoryFeature {

[[nodiscard]] not_null<HistoryItem*> MakeRestoredMessage(
	not_null<::History*> history,
	const History::Record &record,
	MsgId id,
	MessageFlags flags);

} // namespace Serein::HistoryFeature
