#include "serein/hooks/ghost.h"

#include "serein/features/ghost/model/policy.h"

namespace Serein::Hooks {
namespace {

auto ForcedReadReceipts = 0;

[[nodiscard]] bool Allows(
		gsl::not_null<Main::Session*> session,
		Ghost::Activity activity) {
	return Ghost::Allows(Ghost::Read(ForAccount(session)), activity);
}

} // namespace

bool AllowOnline(gsl::not_null<Main::Session*> session) {
	return Allows(session, Ghost::Activity::Online);
}

ForcedReadReceipt::ForcedReadReceipt() {
	++ForcedReadReceipts;
}

ForcedReadReceipt::~ForcedReadReceipt() {
	--ForcedReadReceipts;
}

bool AllowReadReceipt(gsl::not_null<Main::Session*> session) {
	return (ForcedReadReceipts > 0)
		|| Allows(session, Ghost::Activity::ReadReceipt);
}

bool AllowTyping(gsl::not_null<Main::Session*> session) {
	return Allows(session, Ghost::Activity::Typing);
}

bool AllowStoryView(gsl::not_null<Main::Session*> session) {
	return Allows(session, Ghost::Activity::StoryView);
}

bool AllowViewIncrement(gsl::not_null<Main::Session*> session) {
	return Allows(session, Ghost::Activity::ViewIncrement);
}

} // namespace Serein::Hooks
