#include "serein/services/draft_translation.h"

#include "serein/schema/gen/settings/compose.h"
#include "serein/services/translation.h"
#include "boxes/translate_box.h"
#include "boxes/translate_box_content.h"
#include "core/ui_integration.h"
#include "lang/lang_keys.h"
#include "main/session/session_show.h"
#include "ui/layers/generic_box.h"
#include "ui/text/text_utilities.h"
#include "ui/widgets/fields/input_field.h"
#include "ui/widgets/popup_menu.h"

namespace Serein {
namespace {

void ShowDraftTranslation(
		std::shared_ptr<Main::SessionShow> show,
		not_null<Ui::InputField*> field) {
	const auto original = field->getTextWithTags();
	show->showBox(Box([=](not_null<Ui::GenericBox*> box) {
		struct State {
			std::unique_ptr<Ui::TranslateProvider> provider;
			std::optional<TextWithEntities> result;
			rpl::variable<LanguageId> to;
		};
		const auto state = box->lifetime().make_state<State>();
		state->to = Ui::ChooseTranslateTo(LanguageId());
		const auto text = TextWithEntities{
			original.text,
			TextUtilities::ConvertTextTagsToEntities(original.tags),
		};
		Ui::TranslateBoxContent(box, {
			.text = text,
			.textContext = Core::TextContext({ .session = &show->session() }),
			.to = state->to.value(),
			.chooseTo = [=] {
				box->uiShow()->showBox(Ui::ChooseTranslateToBox(
					state->to.current(),
					crl::guard(box, [=](LanguageId to) { state->to = to; })));
			},
			.request = [=](LanguageId to,
					Fn<void(Ui::TranslateBoxContentResult)> done) {
				state->result.reset();
				state->provider = CreateInteractiveTranslateProvider(
					&show->session(), crl::guard(box, [=](QString error) {
						box->showToast(error);
					}));
				state->provider->request({ .text = text }, to,
					crl::guard(box, [=](Ui::TranslateProviderResult response) {
						state->result = response.text;
						done({
							.text = std::move(response.text),
							.error = response.error
								== Ui::TranslateProviderError::None
								? Ui::TranslateBoxContentError::None
								: response.error
									== Ui::TranslateProviderError::LocalLanguagePackMissing
								? Ui::TranslateBoxContentError::LocalLanguagePackMissing
								: Ui::TranslateBoxContentError::Unknown,
						});
					}));
			},
		});
		box->addButton(tr::lng_serein_translate_apply(),
			crl::guard(field, [=] {
				if (!state->result) {
					return;
				}
				if (field->getTextWithTags() != original) {
					box->showToast(tr::lng_serein_draft_changed(tr::now));
					return;
				}
				field->setTextWithTags({
					state->result->text,
					TextUtilities::ConvertEntitiesToTextTags(
						state->result->entities),
				});
				box->closeBox();
			}));
	}));
}

} // namespace

void InstallDraftTranslation(
		not_null<Ui::InputField*> field,
		std::shared_ptr<Main::SessionShow> show) {
	const auto weak = std::weak_ptr<Main::SessionShow>(show);
	field->addContextMenuHook([=](Ui::InputField::ContextMenuRequest request) {
		if (field->empty() || !ForDevice().Get(Compose::kDraftTranslation)) {
			return;
		}
		request.menu->addAction(tr::lng_serein_translate_draft(tr::now),
			field, crl::guard(field, [=] {
				if (const auto locked = weak.lock()) {
					ShowDraftTranslation(locked, field);
				}
			}));
	});
}

} // namespace Serein
