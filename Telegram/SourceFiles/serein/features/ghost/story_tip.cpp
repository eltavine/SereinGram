#include "serein/features/ghost/story_tip.h"

#include "base/flat_set.h"
#include "core/application.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "window/window_controller.h"

namespace Serein::Ghost {

void TipHiddenStoryView(gsl::not_null<Main::Session*> session) {
	static auto tipped = base::flat_set<Main::Session*>();
	if (!tipped.emplace(session).second) {
		return;
	}
	session->lifetime().add([=] { tipped.remove(session); });
	if (const auto window = Core::App().activeWindow()) {
		window->showToast(tr::lng_serein_ghost_story_tip(tr::now));
	}
}

} // namespace Serein::Ghost
