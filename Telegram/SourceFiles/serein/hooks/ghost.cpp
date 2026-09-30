#include "serein/hooks/ghost.h"

#include "serein/features/ghost/model/policy.h"

namespace Serein::Hooks {
namespace {

[[nodiscard]] bool Allows(
		gsl::not_null<Main::Session*> session,
		Ghost::Activity activity) {
	return Ghost::Allows(Ghost::Read(ForAccount(session)), activity);
}

} // namespace

bool AllowOnline(gsl::not_null<Main::Session*> session) {
	return Allows(session, Ghost::Activity::Online);
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
