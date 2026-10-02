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
		const Ghost::Policy &policy,
		Api::SendOptions &options) {
	if (options.scheduled
		|| options.shortcutId
		|| history->peer->isSelf()
		|| !Ghost::ScheduleOutgoing(policy)) {
		return;
	}
	options.scheduled = base::unixtime::now() + kGhostScheduleDelay;
}

} // namespace

void ApplySendOptions(
		gsl::not_null<History*> history,
		Api::SendOptions &options) {
	const auto policy = Ghost::Read(
		ForAccount(&history->session()),
		ForDevice());
	if (Compose::SendSilently() || Ghost::SendSilently(policy)) {
		options.silent = true;
	}
	ApplyGhostSchedule(history, policy, options);
}

} // namespace Serein::Hooks
