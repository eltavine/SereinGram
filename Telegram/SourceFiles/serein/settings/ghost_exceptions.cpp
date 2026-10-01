#include "serein/settings/ghost_exceptions.h"

#include "data/data_peer.h"
#include "data/data_session.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "serein/core/options.h"
#include "serein/features/ghost/model/exceptions.h"
#include "serein/schema/gen/settings/ghost.h"
#include "settings/settings_builder.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/checkbox.h"
#include "ui/widgets/labels.h"
#include "window/window_session_controller.h"
#include "styles/style_layers.h"
#include "styles/style_settings.h"

namespace Serein {
namespace {

[[nodiscard]] QString ChatName(not_null<Main::Session*> session, quint64 id) {
	const auto peer = session->data().peerLoaded(PeerId(id));
	return peer ? peer->name() : QString::number(id);
}

void ReadExceptionsBox(
		not_null<Ui::GenericBox*> box,
		not_null<Main::Session*> session) {
	box->setTitle(tr::lng_serein_ghost_read_exceptions());
	const auto ids = Ghost::ParseExceptions(
		ForAccount(session).Get(Ghost::kReadReceiptExceptions));
	auto checks = std::vector<std::pair<quint64, Ui::Checkbox*>>();
	for (const auto id : ids) {
		const auto check = box->addRow(object_ptr<Ui::Checkbox>(
			box,
			ChatName(session, id),
			true));
		checks.emplace_back(id, check);
	}
	box->addRow(object_ptr<Ui::FlatLabel>(
		box,
		(checks.empty()
			? tr::lng_serein_ghost_read_exceptions_empty()
			: tr::lng_serein_ghost_read_exceptions_about()),
		st::boxDividerLabel));
	if (checks.empty()) {
		box->addButton(tr::lng_close(), [=] { box->closeBox(); });
		return;
	}
	box->addButton(tr::lng_settings_save(), [=] {
		auto value = QString();
		for (const auto &[id, check] : checks) {
			if (check->checked()) {
				value = Ghost::ToggleException(value, id);
			}
		}
		Expects(ForAccount(session).Set(Ghost::kReadReceiptExceptions, value));
		box->closeBox();
	});
	box->addButton(tr::lng_cancel(), [=] { box->closeBox(); });
}

} // namespace

void AddReadExceptionsRow(::Settings::Builder::SectionBuilder &builder) {
	const auto controller = builder.controller();
	const auto session = &controller->session();
	builder.addButton({
		.id = u"serein/ghost/read-exceptions"_q,
		.title = tr::lng_serein_ghost_read_exceptions(),
		.st = &st::settingsButtonNoIcon,
		.label = ForAccount(session).Value(
			Ghost::kReadReceiptExceptions
		) | rpl::map([](const QString &value) {
			const auto count = Ghost::ParseExceptions(value).size();
			return count
				? QString::number(count)
				: tr::lng_serein_config_off(tr::now);
		}),
		.onClick = [=] {
			controller->show(Box(ReadExceptionsBox, session));
		},
		.keywords = { u"ghost"_q, u"read"_q, u"exceptions"_q },
	});
}

} // namespace Serein
