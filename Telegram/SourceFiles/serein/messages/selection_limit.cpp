#include "serein/messages/selection_limit.h"

#include "config.h"
#include "serein/core/options.h"
#include "serein/schema/gen/settings/messages.h"

namespace Serein::Messages {
namespace {

constexpr auto kRaisedSelectionLimit = 1000;

} // namespace

void StartSelectionLimit() {
	if (ForDevice().Get(kRaiseSelectionLimit)) {
		MaxSelectedItems = kRaisedSelectionLimit;
	}
}

} // namespace Serein::Messages
