#include "serein/hooks/messages/double_click.h"

#include "serein/messages/options.h"
#include "base/unixtime.h"
#include "history/history_item.h"

namespace Serein::Messages {

bool EditOnDoubleClick(gsl::not_null<HistoryItem*> item) {
	return ForDevice().Get(kDoubleClickEditsOwn)
		&& item->out()
		&& item->allowsEdit(base::unixtime::now());
}

} // namespace Serein::Messages
