#include "serein/settings/services.h"
#include "serein/settings/rows.h"
#include "serein/settings/home.h"

#include "core/application.h"
#include "lang/lang_keys.h"
#include "serein/hooks/services/system_ai.h"
#include "serein/hooks/services/model.h"
#include "serein/settings/service_editor.h"
#include "serein/settings/services_network.h"
#include "serein/settings/gen/services_rows.h"
#include "serein/settings/page.h"
#include "serein/settings/send_translations.h"
#include "platform/platform_translate_provider.h"
#include "settings/settings_builder.h"
#include "settings/settings_common_session.h"
#include "ui/boxes/confirm_box.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/checkbox.h"
#include "ui/widgets/labels.h"
#include "ui/vertical_list.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"

#include <algorithm>

#include "styles/style_layers.h"
#include "styles/style_settings.h"
#include "styles/style_menu_icons.h"

namespace Serein {
namespace {

using namespace ::Settings;
using namespace ::Settings::Builder;

class ServicesSection final : public Page<ServicesSection> {
public:
	using Page::Page;

	[[nodiscard]] rpl::producer<QString> title() override {
		return tr::lng_serein_services();
	}

	static const SectionBuildMethod kBuild;

};

QString TranslationSelectionName(const std::optional<ServicesConfig> &config) {
	if (!config) {
		return tr::lng_serein_service_invalid(tr::now);
	}
	const auto &id = config->translation;
	if (id.isEmpty()) {
		return tr::lng_serein_inherit(tr::now);
	} else if (id == u"telegram"_q) {
		return u"Telegram"_q;
	} else if (id == u"system"_q) {
		return Platform::IsTranslateProviderAvailable()
			? tr::lng_serein_service_system(tr::now)
			: tr::lng_serein_system_translation_unavailable(tr::now);
	}
	const auto service = FindService(*config, id);
	return service ? service->name : tr::lng_serein_service_invalid(tr::now);
}

struct SourceChoice {
	QString id;
	QString title;
	bool disabled = false;
};

bool SourceBox(
		not_null<Ui::GenericBox*> box,
		ServiceKind kind,
		std::vector<SourceChoice> choices,
		QString ServicesConfig::*selection) {
	const auto current = Services();
	if (!current) {
		box->addRow(object_ptr<Ui::FlatLabel>(
			box, tr::lng_serein_service_invalid(), st::boxLabel));
		return false;
	}
	for (const auto &instance : current->instances) {
		const auto service = ParseService(instance);
		if (service && service->kind == kind) {
			choices.push_back({ service->id, service->name });
		}
	}
	const auto i = ranges::find(
		choices,
		(*current).*selection,
		&SourceChoice::id);
	const auto group = std::make_shared<Ui::RadiobuttonGroup>(
		(i != end(choices)) ? int(i - begin(choices)) : 0);
	for (auto index = 0; index != int(choices.size()); ++index) {
		box->addRow(object_ptr<Ui::Radiobutton>(
			box, group, index, choices[index].title, st::settingsSendType),
			st::settingsSendTypePadding
		)->setDisabled(choices[index].disabled);
	}
	group->setChangedCallback([=](int value) {
		if (!ServicesUnchanged(box, *current)) {
			return;
		}
		auto updated = *current;
		updated.*selection = choices[value].id;
		if (!SetServices(updated)) {
			box->showToast(tr::lng_serein_service_invalid(tr::now));
			return;
		}
		Core::App().saveSettingsDelayed();
		box->closeBox();
	});
	return true;
}

void TranslationSourceBox(not_null<Ui::GenericBox*> box) {
	box->setTitle(tr::lng_serein_service_translation());
	const auto system = Platform::IsTranslateProviderAvailable();
	const auto shown = SourceBox(box, ServiceKind::Translation, {
		{ QString(), tr::lng_serein_inherit(tr::now) },
		{ u"telegram"_q, u"Telegram"_q },
		{ u"system"_q, tr::lng_serein_service_system(tr::now), !system },
	}, &ServicesConfig::translation);
	if (shown && !system) {
		box->addRow(object_ptr<Ui::FlatLabel>(box,
			tr::lng_serein_system_translation_unavailable(), st::boxLabel));
	}
}

QString TranscriptionSelectionName(
		const std::optional<ServicesConfig> &config) {
	if (!config) {
		return tr::lng_serein_service_invalid(tr::now);
	}
	const auto &id = config->transcription;
	if (id.isEmpty() || id == u"telegram"_q) {
		return u"Telegram"_q;
	}
	const auto service = FindService(*config, id);
	return service ? service->name : tr::lng_serein_service_invalid(tr::now);
}

void TranscriptionSourceBox(not_null<Ui::GenericBox*> box) {
	box->setTitle(tr::lng_serein_service_transcription());
	SourceBox(box, ServiceKind::Transcription, {
		{ QString(), u"Telegram"_q },
	}, &ServicesConfig::transcription);
}

const auto kMeta = BuildHelper({
	.id = ServicesSection::Id(),
	.parentId = HomeId(),
	.title = &tr::lng_serein_services,
	.icon = &st::menuIconTranslate,
}, [](SectionBuilder &builder) {
	const auto controller = builder.controller();
	ServiceSettings::AddLayout(builder, {
		.services = [&] {
			builder.addButton({
				.id = u"serein/services/translation"_q,
				.title = tr::lng_serein_service_translation(),
				.st = &st::settingsButtonNoIcon,
				.label = ForDevice().Value(kServicesConfig)
					| rpl::map([](const QByteArray &) {
						return TranslationSelectionName(Services());
					}),
				.onClick = [=] { controller->show(Box(TranslationSourceBox)); },
				.keywords = { u"translation"_q, u"system"_q },
			});
			builder.addButton({
				.id = u"serein/services/transcription"_q,
				.title = tr::lng_serein_service_transcription(),
				.st = &st::settingsButtonNoIcon,
				.label = ForDevice().Value(kServicesConfig)
					| rpl::map([](const QByteArray &) {
						return TranscriptionSelectionName(Services());
					}),
				.onClick = [=] { controller->show(Box(TranscriptionSourceBox)); },
				.keywords = { u"transcription"_q, u"voice"_q },
			});
			builder.addButton({
				.id = u"serein/services/instances"_q,
				.title = tr::lng_serein_services(),
				.st = &st::settingsButtonNoIcon,
				.onClick = [=] {
					const auto current = Services();
					if (!current) {
						controller->show(Ui::MakeInformBox(
							tr::lng_serein_service_invalid(tr::now)));
						return;
					}
					controller->show(Box(ServicesBox, *current));
				},
				.keywords = { u"SereinGram"_q, u"LLM"_q, u"API"_q },
			});
			builder.addDividerText(tr::lng_serein_services_about());
		},
		.preferSystemAi = [&] {
			const auto aiStatus = SystemAiAvailability();
			const auto aiButton = builder.addButton({
				.id = u"serein/services/system-ai"_q,
				.title = tr::lng_serein_system_ai(),
				.st = &st::settingsButtonNoIcon,
				.toggled = ForDevice().Value(kPreferSystemAi),
				.keywords = { u"Apple Intelligence"_q, u"AI"_q },
			});
			if (aiButton) {
				aiButton->setDisabled(aiStatus != 0
					&& !ForDevice().Get(kPreferSystemAi));
				aiButton->toggledChanges(
				) | rpl::on_next([](bool value) {
					Expects(ForDevice().Set(kPreferSystemAi, value));
				}, aiButton->lifetime());
			}
			builder.addDividerText(tr::lng_serein_system_ai_about());
			if (aiStatus != 0) {
				builder.addDividerText(rpl::single(SystemAiStatusText(aiStatus)));
			}
		},
		.sendTranslations = [&] { AddSendTranslations(builder); },
		.proxySubscription = [&] { AddProxySubscriptionRow(builder); },
		.proxyNotes = [&] { AddProxyToolRows(builder); },
		.customDoh = [&] { AddCustomDohRow(builder); },
	});
	AddDatacenterStatusRow(builder);
});

const SectionBuildMethod ServicesSection::kBuild = kMeta.build;

} // namespace

Settings::Type ServicesId() {
	return ServicesSection::Id();
}

} // namespace Serein
