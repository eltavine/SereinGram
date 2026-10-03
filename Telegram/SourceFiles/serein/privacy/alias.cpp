#include "serein/privacy/alias.h"

#include "serein/core/session_lifetime.h"
#include "serein/privacy/options.h"

#include "data/data_peer.h"
#include "data/data_channel.h"
#include "data/data_session.h"
#include "lang/lang_keys.h"
#include "main/main_account.h"
#include "main/main_session.h"
#include "main/session/session_show.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/fields/input_field.h"
#include "ui/widgets/labels.h"
#include "styles/style_layers.h"

#include <map>
#include <memory>

namespace Serein::Privacy {
namespace {

constexpr auto kMaximumAliases = 1000;
constexpr auto kMaximumAliasLength = 96;

struct State {
	PeerAliases aliases;
	bool enabled = false;
	rpl::lifetime lifetime;
};

auto &States() {
	static auto states = std::map<Main::Session*, std::unique_ptr<State>>();
	return states;
}

State &ForSession(not_null<Main::Session*> session) {
	auto &states = States();
	if (const auto i = states.find(session); i != states.end()) {
		return *i->second;
	}
	const auto raw = ForAccount(session).Get(kAliases);
	auto state = std::make_unique<State>();
	state->aliases = ParseAliases(raw).value_or(PeerAliases());
	state->enabled = ForDevice().Get(kLocalNames);
	ForDevice().Value(
		kLocalNames
	) | rpl::skip(1) | rpl::on_next([=, raw = state.get()](bool enabled) {
		raw->enabled = enabled;
		for (const auto &[id, alias] : raw->aliases) {
			if (const auto peer = session->data().peerLoaded(id)) {
				peer->localNameChanged();
			}
		}
	}, state->lifetime);
	const auto inserted = states.emplace(session, std::move(state)).first;
	// History() asks for aliases inside Session(), before its lifetime exists.
	details::BindSessionCleanup(
		session,
		session->account().sessionValue(),
		[session] { States().erase(session); },
		inserted->second->lifetime);
	return *inserted->second;
}

} // namespace

const QString &Alias(not_null<const PeerData*> peer) {
	const auto &state = ForSession(&peer->session());
	const auto i = state.aliases.find(peer->id);
	static const auto empty = QString();
	return (state.enabled && i != state.aliases.end()) ? i->second : empty;
}

const QString &DisplayName(not_null<const PeerData*> peer) {
	if (const auto to = peer->migrateTo()) {
		return DisplayName(to);
	} else if (const auto broadcast = peer->monoforumBroadcast()) {
		return DisplayName(broadcast);
	}
	const auto &alias = Alias(peer);
	return alias.isEmpty() ? peer->name() : alias;
}

QString SetAlias(
		not_null<PeerData*> peer,
		const QString &value,
		const QString &expected) {
	if (!ValidAlias(value)) {
		return tr::lng_serein_alias_invalid(tr::now);
	}
	auto &state = ForSession(&peer->session());
	if (Alias(peer) != expected) {
		return tr::lng_serein_alias_changed(tr::now);
	}
	auto aliases = state.aliases;
	if (value.isEmpty()) {
		aliases.remove(peer->id);
	} else {
		aliases[peer->id] = value;
	}
	if (aliases.size() > kMaximumAliases
		|| !ForAccount(&peer->session()).Set(kAliases, SerializeAliases(aliases))) {
		return tr::lng_serein_alias_invalid(tr::now);
	}
	state.aliases = std::move(aliases);
	peer->localNameChanged();
	return {};
}

void ShowAlias(
		std::shared_ptr<Main::SessionShow> show,
		not_null<PeerData*> peer) {
	show->showBox(Box([=](not_null<Ui::GenericBox*> box) {
		box->setTitle(tr::lng_serein_peer_alias());
		const auto original = Alias(peer);
		box->addRow(object_ptr<Ui::FlatLabel>(
			box, rpl::single(peer->name()), st::boxLabel));
		const auto field = box->addRow(object_ptr<Ui::InputField>(
			box, st::defaultInputField, Ui::InputField::Mode::SingleLine,
			tr::lng_serein_peer_alias(), original));
		field->setMaxLength(kMaximumAliasLength);
		box->addRow(object_ptr<Ui::FlatLabel>(
			box, tr::lng_serein_alias_about(), st::boxLabel));
		box->addButton(tr::lng_settings_save(), [=] {
			const auto error = SetAlias(peer, field->getLastText(), original);
			if (!error.isEmpty()) {
				box->showToast(error);
				field->showError();
				return;
			}
			box->closeBox();
		});
		box->addButton(tr::lng_cancel(), [=] { box->closeBox(); });
	}));
}

} // namespace Serein::Privacy
