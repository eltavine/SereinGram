#include "serein/hooks/ghost.h"

#include "serein/features/ghost/model/policy.h"
#include "serein/features/ghost/story_tip.h"
#include "serein/hooks/gen/ghost.h"

namespace Serein::Hooks {
namespace {

auto ForcedReadReceipts = 0;

[[nodiscard]] bool Allows(
		gsl::not_null<Main::Session*> session,
		Serein::Ghost::Activity activity) {
	return Serein::Ghost::Allows(
		Serein::Ghost::Read(ForAccount(session), ForDevice()),
		activity);
}

} // namespace

QString GhostStatus(
		gsl::not_null<Main::Session*> session,
		const QString &status) {
	const auto prefix = QString::fromUtf8("\xf0\x9f\x91\xbb ");
	const auto plain = status.startsWith(prefix)
		? status.mid(prefix.size())
		: status;
	const auto enabled = Serein::Ghost::Enabled(
		ForAccount(session),
		ForDevice());
	return (enabled && !plain.isEmpty()) ? (prefix + plain) : plain;
}

bool AllowOnline(gsl::not_null<Main::Session*> session) {
	return Allows(session, Serein::Ghost::Activity::Online);
}

ForcedReadReceipt::ForcedReadReceipt() {
	++ForcedReadReceipts;
}

ForcedReadReceipt::~ForcedReadReceipt() {
	--ForcedReadReceipts;
}

ExplicitReadReceipts::ExplicitReadReceipts(
	gsl::not_null<Main::Session*> session)
: _forced(Ghost::GhostExplicitReadReceipts(session)) {
	if (_forced) {
		++ForcedReadReceipts;
	}
}

ExplicitReadReceipts::~ExplicitReadReceipts() {
	if (_forced) {
		--ForcedReadReceipts;
	}
}

bool AllowReadReceipt(gsl::not_null<Main::Session*> session) {
	return (ForcedReadReceipts > 0)
		|| Allows(session, Serein::Ghost::Activity::ReadReceipt);
}

bool AllowTyping(gsl::not_null<Main::Session*> session) {
	return Allows(session, Serein::Ghost::Activity::Typing);
}

bool AllowStoryView(gsl::not_null<Main::Session*> session) {
	const auto allowed = Allows(session, Serein::Ghost::Activity::StoryView);
	if (!allowed) {
		Serein::Ghost::TipHiddenStoryView(session);
	}
	return allowed;
}

bool AllowViewIncrement(gsl::not_null<Main::Session*> session) {
	return Allows(session, Serein::Ghost::Activity::ViewIncrement);
}

} // namespace Serein::Hooks
