#include "serein/hooks/compose/field_menu.h"

#include "serein/compose/mention.h"
#include "serein/compose/options.h"
#include "serein/compose/text_replacements.h"
#include "serein/hooks/compose/text.h"
#include "serein/services/draft_translation.h"
#include "ui/widgets/fields/input_field.h"

namespace Serein::Hooks::Compose {

void InstallFieldMenu(
		not_null<Ui::InputField*> field,
		std::shared_ptr<Main::SessionShow> show) {
	Serein::Compose::InstallQuickReplies(field);
	crl::on_main(field, [=] {
		ForDevice().Value(
			Serein::Compose::kTextReplacements
		) | rpl::on_next([=](const QByteArray &raw) {
			auto replaces = Ui::InstantReplaces::Default();
			const auto rules = Serein::Compose::ReadTextReplacements(raw);
			for (const auto &rule : rules ? rules->rules : std::vector<
					Serein::Compose::TextReplacement>()) {
				replaces.add(rule.from, rule.to);
			}
			field->setInstantReplaces(replaces);
		}, field->lifetime());
	});
	if (show) {
		Serein::InstallDraftTranslation(field, show);
		Serein::Compose::InstallMention(field, std::move(show));
	}
}

} // namespace Serein::Hooks::Compose
