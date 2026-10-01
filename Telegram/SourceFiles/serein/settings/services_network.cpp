#include "serein/settings/services_network.h"

#include "lang/lang_keys.h"
#include "serein/core/options.h"
#include "serein/network/proxy_import.h"
#include "serein/network/proxy_tools.h"
#include "serein/schema/gen/settings/services.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/fields/input_field.h"
#include "ui/widgets/labels.h"
#include "window/window_session_controller.h"
#include "styles/style_layers.h"
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

} // namespace

void AddNetworkSettings(::Settings::Builder::SectionBuilder &builder) {
	const auto controller = builder.controller();
	builder.addButton({
		.id = u"serein/services/proxy-subscription"_q,
		.title = tr::lng_serein_proxy_subscription(),
		.st = &st::settingsButtonNoIcon,
		.label = ForDevice().Value(ServiceSettings::kProxySubscription)
			| rpl::map([](const QString &url) {
				return url.isEmpty()
					? tr::lng_serein_config_off(tr::now)
					: QUrl(url).host();
			}),
		.onClick = [=] { controller->show(Box(ProxySubscriptionBox)); },
		.keywords = { u"proxy"_q, u"subscription"_q, u"MTProto"_q },
	});
	builder.addDividerText(tr::lng_serein_proxy_subscription_about());
	builder.addButton({
		.id = u"serein/services/proxy-sort"_q,
		.title = tr::lng_serein_proxy_sort(),
		.st = &st::settingsButtonNoIcon,
		.onClick = [=] { Network::SortProxiesByLatency(controller->uiShow()); },
		.keywords = { u"proxy"_q, u"ping"_q, u"latency"_q },
	});
	builder.addButton({
		.id = u"serein/services/proxy-clean"_q,
		.title = tr::lng_serein_proxy_clean(),
		.st = &st::settingsButtonNoIcon,
		.onClick = [=] {
			Network::RemoveUnavailableProxies(controller->uiShow());
		},
		.keywords = { u"proxy"_q, u"unavailable"_q, u"clean"_q },
	});
	builder.addDividerText(tr::lng_serein_proxy_tools_about());
	const auto vpnButton = builder.addButton({
		.id = u"serein/services/proxy-vpn"_q,
		.title = tr::lng_serein_proxy_vpn(),
		.st = &st::settingsButtonNoIcon,
		.toggled = ForDevice().Value(ServiceSettings::kPauseProxyOnVpn),
		.keywords = { u"proxy"_q, u"VPN"_q },
	});
	if (vpnButton) {
		vpnButton->toggledChanges(
		) | rpl::on_next([](bool value) {
			Expects(ForDevice().Set(ServiceSettings::kPauseProxyOnVpn, value));
		}, vpnButton->lifetime());
	}
	builder.addDividerText(tr::lng_serein_proxy_vpn_about());
	builder.addButton({
		.id = u"serein/services/custom-doh"_q,
		.title = tr::lng_serein_custom_doh(),
		.st = &st::settingsButtonNoIcon,
		.label = ForDevice().Value(ServiceSettings::kCustomDoh)
			| rpl::map([](const QString &host) {
				return host.isEmpty()
					? tr::lng_serein_config_off(tr::now)
					: host;
			}),
		.onClick = [=] { controller->show(Box(CustomDohBox)); },
		.keywords = { u"DNS"_q, u"DoH"_q, u"censorship"_q },
	});
	builder.addDividerText(tr::lng_serein_custom_doh_about());
}

} // namespace Serein
