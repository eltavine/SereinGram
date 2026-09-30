#include "serein/hooks/ghost.h"

#include "serein/features/ghost/model/policy.h"
#include "apiwrap.h"
#include "base/call_delayed.h"
#include "data/data_histories.h"
#include "data/data_session.h"
#include "history/history.h"
#include "main/main_session.h"

namespace Serein::Hooks {
namespace {

constexpr auto kOfflineAfterSendingDelay = crl::time(1000);

} // namespace

void OnSendingMessage(gsl::not_null<History*> history) {
	const auto session = &history->session();
	const auto policy = Ghost::Read(ForAccount(session));
	if (Ghost::MarkReadAfterSending(policy)) {
		const auto forced = ForcedReadReceipt();
		session->data().histories().readInbox(history);
	}
	if (Ghost::OfflineAfterSending(policy)) {
		base::call_delayed(kOfflineAfterSendingDelay, session, [=] {
			session->api().request(MTPaccount_UpdateStatus(
				MTP_bool(true)
			)).send();
		});
	}
}

} // namespace Serein::Hooks
