#include "serein/hooks/media/story_menu.h"

#include "serein/features/stories/editor.h"
#include "serein/hooks/gen/media.h"
#include "data/data_peer.h"
#include "data/data_story.h"
#include "lang/lang_keys.h"
#include "ui/widgets/menu/menu_add_action_callback.h"
#include "styles/style_serein.h"

namespace Serein::Hooks::Media {

void FillStoryMenu(
		const Ui::Menu::MenuCallback &addAction,
		std::shared_ptr<ChatHelpers::Show> show,
		Data::Story *story) {
	if (!story || !StoryPosting() || !story->peer()->canEditStories()) {
		return;
	}
	const auto peer = story->peer();
	const auto id = story->id();
	addAction(tr::lng_serein_story_edit(tr::now), [=] {
		Serein::Stories::EditStory(show, peer, id);
	}, &st::sereinMediaMenuIconEdit);
}

} // namespace Serein::Hooks::Media
