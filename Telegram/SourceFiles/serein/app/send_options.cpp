#include "serein/hooks/send.h"

#include "serein/features/ghost/model/policy.h"
#include "serein/hooks/gen/compose.h"
#include "api/api_common.h"
#include "base/unixtime.h"
#include "data/data_peer.h"
#include "history/history.h"
#include "main/main_session.h"

namespace Serein::Hooks {
namespace {

constexpr auto kGhostScheduleDelay = TimeId(12);

void ApplyGhostSchedule(
		gsl::not_null<History*> history,
		Api::SendOptions &options) {
	if (options.scheduled
		|| options.shortcutId
		|| history->peer->isSelf()
		|| !Ghost::ScheduleOutgoing(Ghost::Read(ForAccount(&history->session())))) {
		return;
	}
	options.scheduled = base::unixtime::now() + kGhostScheduleDelay;
}

} // namespace

void ApplySendOptions(
		gsl::not_null<History*> history,
		Api::SendOptions &options) {
	if (Compose::SendSilently()) {
		options.silent = true;
	}
	ApplyGhostSchedule(history, options);
}

} // namespace Serein::Hooks
