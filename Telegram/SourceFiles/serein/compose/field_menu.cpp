#include "serein/hooks/compose/field_menu.h"

#include "serein/compose/mention.h"
#include "serein/hooks/compose/text.h"
#include "serein/services/draft_translation.h"

namespace Serein::Hooks::Compose {

void InstallFieldMenu(
		not_null<Ui::InputField*> field,
		std::shared_ptr<Main::SessionShow> show) {
	Serein::Compose::InstallQuickReplies(field);
	if (show) {
		Serein::InstallDraftTranslation(field, show);
		Serein::Compose::InstallMention(field, std::move(show));
	}
}

} // namespace Serein::Hooks::Compose
