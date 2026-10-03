#include "serein/settings/send_translations.h"

#include "serein/core/options.h"
#include "serein/schema/gen/settings/services.h"
#include "serein/services/send_translation.h"
#include "data/data_peer.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "settings/settings_builder.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/labels.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"

#include "styles/style_layers.h"
#include "styles/style_settings.h"

namespace Serein {
namespace {

[[nodiscard]] QString RowText(not_null<PeerData*> peer) {
	const auto name = peer->name().isEmpty()
		? tr::lng_deleted(tr::now)
		: peer->name();
	return name + u" \u2014 "_q + SendTranslationName(peer);
}

void SendTranslationsBox(
		not_null<Ui::GenericBox*> box,
		not_null<Window::SessionController*> controller) {
	box->setTitle(tr::lng_serein_send_translation());
	const auto session = &controller->session();
	const auto rows = box->addRow(object_ptr<Ui::VerticalLayout>(box));
	ForAccount(session).Value(
		ServiceSettings::kSendTranslations
	) | rpl::to_empty | rpl::on_next([=] {
		rows->clear();
		const auto peers = SendTranslationPeers(session);
		if (peers.empty()) {
			rows->add(object_ptr<Ui::FlatLabel>(
				rows,
				tr::lng_serein_send_translation_empty(),
				st::boxLabel));
		}
		for (const auto &peer : peers) {
			const auto row = rows->add(object_ptr<Ui::SettingsButton>(
				rows,
				rpl::single(RowText(peer)),
				st::settingsButtonNoIcon));
			row->setClickedCallback([=] {
				ChooseSendTranslation(controller->uiShow(), peer);
			});
		}
		rows->resizeToWidth(box->width());
	}, box->lifetime());
	box->addRow(object_ptr<Ui::FlatLabel>(
		box,
		tr::lng_serein_send_translation_about(),
		st::boxDividerLabel));
	box->addButton(tr::lng_close(), [=] { box->closeBox(); });
}

} // namespace

void AddSendTranslations(::Settings::Builder::SectionBuilder &builder) {
	const auto controller = builder.controller();
	const auto session = builder.session();
	builder.addButton({
		.id = u"serein/services/send-translation"_q,
		.title = tr::lng_serein_send_translation(),
		.st = &st::settingsButtonNoIcon,
		.label = ForAccount(session).Value(
			ServiceSettings::kSendTranslations
		) | rpl::map([=](const QByteArray &) {
			const auto count = SendTranslationPeers(session).size();
			return count
				? QString::number(count)
				: tr::lng_serein_config_off(tr::now);
		}),
		.onClick = [=] {
			controller->show(Box(SendTranslationsBox, controller));
		},
		.keywords = { u"translate"_q, u"send"_q, u"outgoing"_q },
	});
}

} // namespace Serein
