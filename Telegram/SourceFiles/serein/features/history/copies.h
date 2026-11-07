#pragma once

#include "serein/ports/history_store.h"

#include <gsl/pointers>

class HistoryItem;

namespace Serein::HistoryFeature {

struct CopyInfo {
	History::Record record;
	bool version = false;
	bool deleted = false;
	bool edited = false;
};

void RegisterCopy(gsl::not_null<const HistoryItem*> item, CopyInfo info);
void UnregisterCopy(gsl::not_null<const HistoryItem*> item);
[[nodiscard]] const CopyInfo *FindCopy(gsl::not_null<const HistoryItem*> item);

} // namespace Serein::HistoryFeature
