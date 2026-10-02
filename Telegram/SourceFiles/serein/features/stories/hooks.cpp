#include "serein/hooks/media/story_menu.h"

#include "serein/features/stories/composer.h"
#include "serein/features/stories/editor.h"
#include "serein/hooks/gen/media.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "data/data_stories.h"
#include "data/data_story.h"
#include "lang/lang_keys.h"
#include "ui/widgets/menu/menu_add_action_callback.h"
#include "styles/style_menu_icons.h"
#include "styles/style_serein.h"

namespace Serein::Hooks::Media {

void FillStoryMenu(
		const Ui::Menu::MenuCallback &addAction,
		std::shared_ptr<ChatHelpers::Show> show,
		Data::Story *story) {
	if (!story || !StoryPosting()) {
		return;
	}
	const auto peer = story->peer();
	const auto id = story->id();
	if (peer->canEditStories()) {
		addAction(tr::lng_serein_story_edit(tr::now), [=] {
			Serein::Stories::EditStory(show, peer, id);
		}, &st::sereinMediaMenuIconEdit);
	} else if (story->canShare()) {
		addAction(tr::lng_serein_story_repost(tr::now), [=] {
			const auto found = peer->owner().stories().lookup({ peer->id, id });
			if (found) {
				Serein::Stories::StartRepost(show, *found);
			}
		}, &st::mediaMenuIconShare);
	}
}

} // namespace Serein::Hooks::Media
