#include "serein/settings/keyword_alerts.h"

#include "serein/core/options.h"
#include "serein/features/keyword_alerts/model/keywords.h"
#include "serein/schema/gen/settings/filters.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "settings/settings_builder.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/checkbox.h"
#include "ui/widgets/fields/input_field.h"
#include "ui/widgets/labels.h"
#include "window/window_session_controller.h"

#include "styles/style_layers.h"
#include "styles/style_settings.h"

#include <algorithm>

namespace Serein::Filters {
namespace {

using Notifications::KeywordAlerts;

constexpr auto kMaxLine = 260;

[[nodiscard]] KeywordAlerts Read(const QByteArray &raw) {
	return Notifications::ReadKeywordAlerts(raw).value_or(KeywordAlerts());
}

[[nodiscard]] QString Describe(const KeywordAlerts &config) {
	return (config.enabled && !config.rules.empty())
		? QString::number(config.rules.size())
		: tr::lng_serein_config_off(tr::now);
}

void KeywordAlertsBox(
		not_null<Ui::GenericBox*> box,
		not_null<Main::Session*> session) {
	box->setTitle(tr::lng_serein_keyword_alerts());
	const auto config = Read(ForAccount(session).Get(kKeywordAlerts));
	const auto enabled = box->addRow(object_ptr<Ui::Checkbox>(
		box,
		tr::lng_serein_keyword_alerts_enable(tr::now),
		config.enabled));
	const auto input = box->addRow(object_ptr<Ui::InputField>(
		box,
		st::defaultInputField,
		Ui::InputField::Mode::MultiLine,
		tr::lng_serein_keyword_alerts_placeholder(),
		Notifications::FormatKeywordLines(config.rules)));
	input->setMaxLength(Notifications::kMaxKeywordRules * kMaxLine);
	const auto channels = box->addRow(object_ptr<Ui::Checkbox>(
		box,
		tr::lng_serein_keyword_alerts_channels(tr::now),
		config.includeChannels));
	box->addRow(object_ptr<Ui::FlatLabel>(
		box,
		tr::lng_serein_keyword_alerts_about(),
		st::boxDividerLabel));
	box->setFocusCallback([=] { input->setFocusFast(); });

	box->addButton(tr::lng_settings_save(), [=] {
		auto result = KeywordAlerts{
			.enabled = enabled->checked(),
			.rules = Notifications::ParseKeywordLines(input->getLastText()),
			.includeChannels = channels->checked(),
		};
		const auto invalid = std::ranges::find_if(
			result.rules,
			[](const auto &rule) {
				return !Notifications::ValidKeywordRule(rule);
			});
		if (result.rules.size() > Notifications::kMaxKeywordRules) {
			box->showToast(tr::lng_serein_keyword_alerts_too_many(
				tr::now,
				lt_limit,
				QString::number(Notifications::kMaxKeywordRules)));
			return;
		} else if (invalid != result.rules.end()) {
			box->showToast(tr::lng_serein_keyword_alerts_invalid(
				tr::now,
				lt_pattern,
				invalid->pattern));
			return;
		}
		const auto raw = (result == KeywordAlerts())
			? QByteArray()
			: Notifications::SerializeKeywordAlerts(result);
		Expects(ForAccount(session).Set(kKeywordAlerts, raw));
		box->closeBox();
	});
	box->addButton(tr::lng_cancel(), [=] { box->closeBox(); });
}

} // namespace

void AddKeywordAlerts(::Settings::Builder::SectionBuilder &builder) {
	const auto controller = builder.controller();
	const auto session = builder.session();
	builder.addButton({
		.id = u"serein/rules/keyword-alerts"_q,
		.title = tr::lng_serein_keyword_alerts(),
		.st = &st::settingsButtonNoIcon,
		.label = ForAccount(session).Value(kKeywordAlerts)
			| rpl::map([](QByteArray raw) { return Describe(Read(raw)); }),
		.onClick = [=] { controller->show(Box(KeywordAlertsBox, session)); },
		.keywords = { u"keyword"_q, u"alert"_q, u"mention"_q },
	});
}

} // namespace Serein::Filters
