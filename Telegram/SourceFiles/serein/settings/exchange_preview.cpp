#include "serein/settings/exchange_preview.h"

#include "serein/hooks/core/language.h"
#include "serein/settings/restart.h"
#include "lang/lang_instance.h"
#include "lang_auto_counts.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/labels.h"
#include "window/window_session_controller.h"
#include "styles/style_layers.h"

namespace Serein {
namespace {

constexpr auto kPreviewLength = 120;
constexpr auto kSkippedShown = 20;

[[nodiscard]] const OptionInfo *Find(const QString &key) {
	const auto encoded = key.toUtf8();
	return RegisteredOptions().Find(std::string_view(
		encoded.constData(), encoded.size()));
}

[[nodiscard]] QString ValueText(const OptionInfo &info, const QByteArray &raw) {
	const auto value = raw.isEmpty() ? info.fallbackRaw : raw;
	if (info.type == OptionInfo::ValueType::Boolean) {
		return (value == "1")
			? tr::lng_serein_config_on(tr::now)
			: tr::lng_serein_config_off(tr::now);
	}
	auto result = (info.type == OptionInfo::ValueType::String)
		? QString::fromUtf8(value.mid(1))
		: QString::fromUtf8(value);
	if (result.size() > kPreviewLength) {
		result.truncate(kPreviewLength);
		if (result.back().isHighSurrogate()) {
			result.chop(1);
		}
		result += QChar(0x2026);
	}
	return result;
}

[[nodiscard]] bool NeedsRestart(const ExchangePlan &plan) {
	return ranges::any_of(plan.changes, [](const ExchangeChange &change) {
		const auto info = Find(change.key);
		return info
			&& (info->flags & static_cast<unsigned>(Flag::RequiresRestart));
	});
}

} // namespace

void AddSelectableText(not_null<Ui::GenericBox*> box, const QString &text) {
	const auto label = box->addRow(object_ptr<Ui::FlatLabel>(
		box, text, st::boxLabel));
	label->setSelectable(true);
	label->setBreakEverywhere(true);
}

QString LocalizedKey(std::string_view key) {
	const auto bytes = QByteArray(key.data(), key.size());
	const auto index = Lang::GetKeyIndex(QLatin1String(bytes));
	return (index == Lang::kKeysCount)
		? QString::fromUtf8(bytes)
		: LocalizedValue(Lang::GetInstance(), index);
}

QString OptionTitle(const OptionInfo &info) {
	return LocalizedKey(info.titleKey);
}

QString ScopedOptionTitle(const OptionInfo &info) {
	return (info.scope == Scope::Account)
		? tr::lng_serein_config_account_option(
			tr::now,
			lt_title,
			OptionTitle(info))
		: OptionTitle(info);
}

Options *AccountOptions(not_null<Window::SessionController*> controller) {
	return &ForAccount(&controller->session());
}

void ShowExchangePreview(
		not_null<Window::SessionController*> controller,
		const ExchangePlan &plan,
		const ExchangePreviewTexts &texts) {
	if (!plan.error.isEmpty()) {
		controller->showToast(plan.error);
		return;
	}
	controller->show(Box([=](not_null<Ui::GenericBox*> box) {
		box->setTitle(texts.title());
		AddSelectableText(box, texts.about(tr::now));
		AddSelectableText(box, tr::lng_serein_config_changes(
			tr::now, lt_amount, QString::number(plan.changes.size())));
		for (const auto &change : plan.changes) {
			if (const auto info = Find(change.key)) {
				AddSelectableText(box, ScopedOptionTitle(*info) + u"\n"_q
					+ ValueText(*info, change.before)
					+ u" → "_q + ValueText(*info, change.after));
			}
		}
		if (!plan.skippedKeys.isEmpty()) {
			AddSelectableText(box, tr::lng_serein_config_unknown(
				tr::now, lt_amount, QString::number(plan.skippedKeys.size()))
				+ u"\n"_q + plan.skippedKeys.mid(0, kSkippedShown).join('\n'));
		}
		if (!plan.changes.empty()) {
			box->addButton(texts.apply(), crl::guard(controller, [=] {
				const auto result = Exchange::Apply(
					ForDevice(),
					AccountOptions(controller),
					RegisteredOptions(),
					plan);
				if (!result.applied) {
					box->showToast(result.error);
					return;
				}
				box->closeBox();
				controller->showToast(texts.done(tr::now));
				if (NeedsRestart(plan)) {
					ShowRestartPrompt(controller);
				}
			}));
		}
		box->addButton(tr::lng_cancel(), [=] { box->closeBox(); });
	}));
}

} // namespace Serein
