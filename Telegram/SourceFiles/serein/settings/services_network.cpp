#include "serein/settings/services_network.h"
#include "serein/settings/rows.h"

#include "core/application.h"
#include "base/flat_set.h"
#include "core/core_settings.h"
#include "lang/lang_keys.h"
#include "mtproto/mtproto_proxy_data.h"
#include "serein/core/options.h"
#include "serein/network/dc_status.h"
#include "serein/network/proxy_import.h"
#include "serein/network/proxy_notes.h"
#include "serein/network/proxy_tools.h"
#include "serein/schema/gen/settings/services.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/fields/input_field.h"
#include "ui/widgets/labels.h"
#include "window/window_session_controller.h"
#include "styles/style_layers.h"
#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"

#include <QtCore/QUrl>

namespace Serein {
namespace {

void CustomDohBox(not_null<Ui::GenericBox*> box) {
	box->setTitle(tr::lng_serein_custom_doh());
	box->addRow(object_ptr<Ui::FlatLabel>(
		box,
		tr::lng_serein_custom_doh_hint(),
		st::boxLabel));
	const auto field = box->addRow(object_ptr<Ui::InputField>(
		box,
		st::defaultInputField,
		rpl::single(u"dns.alidns.com"_q),
		ForDevice().Get(ServiceSettings::kCustomDoh)));
	field->setMaxLength(253);
	box->setFocusCallback([=] { field->setFocusFast(); });
	const auto save = [=] {
		const auto host = field->getLastText().trimmed().toLower();
		if (!ForDevice().Set(ServiceSettings::kCustomDoh, host)) {
			field->showError();
			return;
		}
		box->closeBox();
	};
	field->submits(
	) | rpl::on_next([=](auto) { save(); }, field->lifetime());
	box->addButton(tr::lng_settings_save(), save);
	box->addButton(tr::lng_cancel(), [=] { box->closeBox(); });
}

void ProxySubscriptionBox(not_null<Ui::GenericBox*> box) {
	box->setTitle(tr::lng_serein_proxy_subscription());
	box->addRow(object_ptr<Ui::FlatLabel>(
		box,
		tr::lng_serein_proxy_subscription_about(),
		st::boxLabel));
	const auto field = box->addRow(object_ptr<Ui::InputField>(
		box,
		st::defaultInputField,
		Ui::InputField::Mode::SingleLine,
		rpl::single(u"https://"_q),
		ForDevice().Get(ServiceSettings::kProxySubscription)));
	field->setMaxLength(2048);
	box->setFocusCallback([=] { field->setFocusFast(); });
	const auto save = [=] {
		const auto url = field->getLastText().trimmed();
		if (!ForDevice().Set(ServiceSettings::kProxySubscription, url)) {
			field->showError();
			return false;
		}
		return true;
	};
	box->addButton(tr::lng_serein_proxy_subscription_update(), [=] {
		if (save() && !field->getLastText().trimmed().isEmpty()) {
			Network::UpdateProxySubscription(box->uiShow());
			box->closeBox();
		}
	});
	box->addButton(tr::lng_settings_save(), [=] {
		if (save()) {
			box->closeBox();
		}
	});
	box->addButton(tr::lng_cancel(), [=] { box->closeBox(); });
}

[[nodiscard]] QString ProxyTitle(const MTP::ProxyData &proxy) {
	using Type = MTP::ProxyData::Type;
	const auto type = (proxy.type == Type::Socks5) ? u"SOCKS5"_q
		: (proxy.type == Type::Http) ? u"HTTP"_q
		: (proxy.type == Type::Mtproto) ? u"MTProto"_q
		: u"Web"_q;
	return (proxy.type == Type::Web)
		? (type + u' ' + proxy.host)
		: (type + u' ' + proxy.host + u':' + QString::number(proxy.port));
}

void ProxyNotesBox(not_null<Ui::GenericBox*> box) {
	box->setTitle(tr::lng_serein_proxy_notes());
	box->addRow(object_ptr<Ui::FlatLabel>(
		box,
		tr::lng_serein_proxy_notes_about(),
		st::boxLabel));
	const auto &list = Core::App().settings().proxy().list();
	if (list.empty()) {
		box->addRow(object_ptr<Ui::FlatLabel>(
			box,
			tr::lng_serein_proxy_notes_empty(),
			st::boxLabel));
		box->addButton(tr::lng_close(), [=] { box->closeBox(); });
		return;
	}
	const auto current = Network::ParseProxyNotes(
		ForDevice().Get(ServiceSettings::kProxyNotes)
	).value_or(Network::ProxyNotes());
	auto fields = std::vector<std::pair<QString, Ui::InputField*>>();
	auto seen = base::flat_set<QString>();
	for (const auto &proxy : list) {
		const auto key = Network::ProxyNoteKey(proxy.host, proxy.port);
		if (int(fields.size()) >= Network::kMaxProxyNotes
			|| !seen.insert(key).second) {
			continue;
		}
		const auto i = current.find(key);
		const auto field = box->addRow(object_ptr<Ui::InputField>(
			box,
			st::defaultInputField,
			Ui::InputField::Mode::SingleLine,
			rpl::single(ProxyTitle(proxy)),
			(i != current.end()) ? i->second : QString()));
		field->setMaxLength(Network::kMaxProxyNoteLength);
		fields.emplace_back(key, field);
	}
	box->addButton(tr::lng_settings_save(), [=] {
		auto notes = Network::ProxyNotes();
		for (const auto &[key, field] : fields) {
			const auto note = field->getLastText().trimmed();
			if (note.isEmpty()) {
				continue;
			} else if (!Network::ValidProxyNote(note)) {
				field->showError();
				return;
			}
			notes.emplace(key, note);
		}
		if (!ForDevice().Set(
				ServiceSettings::kProxyNotes,
				Network::SerializeProxyNotes(notes))) {
			box->showToast(tr::lng_serein_proxy_notes_invalid(tr::now));
			return;
		}
		box->closeBox();
	});
	box->addButton(tr::lng_cancel(), [=] { box->closeBox(); });
}

} // namespace

void AddProxySubscriptionRow(::Settings::Builder::SectionBuilder &builder) {
	const auto controller = builder.controller();
	AddRow(builder, {
		.id = u"serein/services/proxy-subscription"_q,
		.title = tr::lng_serein_proxy_subscription(),
		.label = ForDevice().Value(ServiceSettings::kProxySubscription)
			| rpl::map([](const QString &url) {
				return url.isEmpty()
					? tr::lng_serein_config_off(tr::now)
					: QUrl(url).host();
			}),
		.onClick = [=] { controller->show(Box(ProxySubscriptionBox)); },
		.keywords = { u"proxy"_q, u"subscription"_q, u"MTProto"_q },
		.visual = {
			.icon = &st::menuIconLink,
			.about = tr::lng_serein_proxy_subscription_row_about,
		},
	});
	AddNote(builder, tr::lng_serein_proxy_subscription_about);
}

void AddProxyToolRows(::Settings::Builder::SectionBuilder &builder) {
	const auto controller = builder.controller();
	AddRow(builder, {
		.id = u"serein/services/proxy-sort"_q,
		.title = tr::lng_serein_proxy_sort(),
		.onClick = [=] { Network::SortProxiesByLatency(controller->uiShow()); },
		.keywords = { u"proxy"_q, u"ping"_q, u"latency"_q },
		.visual = {
			.icon = &st::menuIconOrderNumber,
			.about = tr::lng_serein_proxy_sort_about,
		},
	});
	AddRow(builder, {
		.id = u"serein/services/proxy-clean"_q,
		.title = tr::lng_serein_proxy_clean(),
		.onClick = [=] {
			Network::RemoveUnavailableProxies(controller->uiShow());
		},
		.keywords = { u"proxy"_q, u"unavailable"_q, u"clean"_q },
		.visual = {
			.icon = &st::menuIconClear,
			.about = tr::lng_serein_proxy_clean_about,
		},
	});
	AddRow(builder, {
		.id = u"serein/services/proxy-notes"_q,
		.title = tr::lng_serein_proxy_notes(),
		.onClick = [=] { controller->show(Box(ProxyNotesBox)); },
		.keywords = { u"proxy"_q, u"note"_q, u"remark"_q },
		.visual = {
			.icon = &st::menuIconTagEdit,
			.about = tr::lng_serein_proxy_notes_row_about,
		},
	});
	AddNote(builder, tr::lng_serein_proxy_tools_about);
}

void AddCustomDohRow(::Settings::Builder::SectionBuilder &builder) {
	const auto controller = builder.controller();
	AddRow(builder, {
		.id = u"serein/services/custom-doh"_q,
		.title = tr::lng_serein_custom_doh(),
		.label = ForDevice().Value(ServiceSettings::kCustomDoh)
			| rpl::map([](const QString &host) {
				return host.isEmpty()
					? tr::lng_serein_config_off(tr::now)
					: host;
			}),
		.onClick = [=] { controller->show(Box(CustomDohBox)); },
		.keywords = { u"DNS"_q, u"DoH"_q, u"censorship"_q },
		.visual = {
			.icon = &st::menuIconLock,
			.about = tr::lng_serein_custom_doh_row_about,
		},
	});
	AddNote(builder, tr::lng_serein_custom_doh_about);
}

void AddDatacenterStatusRow(::Settings::Builder::SectionBuilder &builder) {
	const auto controller = builder.controller();
	AddRow(builder, {
		.id = u"serein/services/dc-status"_q,
		.title = tr::lng_serein_dc_status(),
		.onClick = [=] { Network::ShowDatacenterStatus(controller); },
		.keywords = { u"DC"_q, u"ping"_q, u"datacenter"_q },
		.visual = {
			.icon = &st::menuIconStats,
			.about = tr::lng_serein_dc_status_row_about,
		},
	});
}

} // namespace Serein
