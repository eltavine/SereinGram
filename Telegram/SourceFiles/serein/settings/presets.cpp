#include "serein/settings/presets.h"

#include "serein/features/presets/model/catalog.h"
#include "serein/schema/gen/settings/interface.h"
#include "serein/settings/exchange_preview.h"
#include "lang/lang_keys.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/labels.h"
#include "window/window_session_controller.h"
#include "styles/style_layers.h"
#include "styles/style_settings.h"
#include "styles/style_widgets.h"

#include <QtCore/QFile>

#include <string>

namespace Serein::Presets {
namespace {

constexpr auto kMaxResourceBytes = 256 * 1024;

[[nodiscard]] QByteArray ReadResource(const QString &name) {
	auto file = QFile(u":/serein/presets/"_q + name);
	return file.open(QIODevice::ReadOnly)
		? file.read(kMaxResourceBytes)
		: QByteArray();
}

[[nodiscard]] QString PresetText(const QString &id, const char *suffix) {
	return LocalizedKey(
		"lng_serein_preset_" + id.toStdString() + std::string(suffix));
}

void Apply(
		not_null<Window::SessionController*> controller,
		const QString &id) {
	const auto plan = Exchange::PlanImport(
		ForDevice(),
		AccountOptions(controller),
		RegisteredOptions(),
		ReadResource(id + u".json"_q));
	if (plan.error.isEmpty() && plan.changes.empty()) {
		controller->showToast(tr::lng_serein_presets_in_place(tr::now));
		return;
	}
	ShowExchangePreview(controller, plan, {
		.title = tr::lng_serein_presets_preview,
		.about = tr::lng_serein_presets_preview_about,
		.apply = tr::lng_serein_presets_apply,
		.done = tr::lng_serein_presets_applied,
	});
}

void FillPresets(
		not_null<Ui::GenericBox*> box,
		not_null<Window::SessionController*> controller,
		bool welcome) {
	box->setTitle(tr::lng_serein_presets());
	AddSelectableText(box, welcome
		? tr::lng_serein_presets_welcome(tr::now)
		: tr::lng_serein_presets_about(tr::now));
	const auto ids = ParseCatalog(ReadResource(u"catalog.json"_q));
	if (!ids) {
		AddSelectableText(box, tr::lng_serein_presets_unavailable(tr::now));
	}
	for (const auto &id : ids.value_or(std::vector<QString>())) {
		const auto button = box->addRow(
			object_ptr<Ui::SettingsButton>(
				box,
				rpl::single(PresetText(id, "")),
				st::settingsButtonNoIcon),
			style::margins());
		button->setClickedCallback(crl::guard(controller, [=] {
			Apply(controller, id);
		}));
		box->addRow(object_ptr<Ui::FlatLabel>(
			box,
			PresetText(id, "_about"),
			st::boxDividerLabel));
	}
	box->addButton(
		welcome ? tr::lng_serein_presets_skip() : tr::lng_close(),
		[=] { box->closeBox(); });
}

} // namespace

void ShowPresets(not_null<Window::SessionController*> controller) {
	controller->show(Box(FillPresets, controller, false));
}

void OfferPresetsOnce(not_null<Window::SessionController*> controller) {
	auto &device = ForDevice();
	if (device.Get(Interface::kPresetsOffered)
		|| !device.Set(Interface::kPresetsOffered, true)) {
		return;
	}
	controller->show(Box(FillPresets, controller, true));
}

} // namespace Serein::Presets
