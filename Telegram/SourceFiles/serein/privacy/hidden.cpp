#include "serein/privacy/hidden.h"

#include "serein/core/id_list.h"
#include "serein/hooks/privacy/hidden.h"
#include "serein/privacy/options.h"
#include "core/application.h"
#include "data/data_channel.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "data/data_user.h"
#include "history/history.h"
#include "lang/lang_keys.h"
#include "main/main_account.h"
#include "main/main_domain.h"
#include "main/main_session.h"
#include "settings.h"
#include "window/notifications_manager.h"
#include "storage/storage_domain.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/fields/password_input.h"
#include "ui/widgets/labels.h"
#include "ui/widgets/menu/menu_add_action_callback.h"
#include "window/window_controller.h"
#include "window/window_session_controller.h"

#include <openssl/crypto.h>

#include <set>

#include "styles/style_layers.h"
#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"
#include "styles/style_widgets.h"

namespace Serein::Privacy {
namespace {

[[nodiscard]] std::set<quint64> Ids(const QString &value) {
	const auto ids = ParseIdList(value, 1000).value_or(std::vector<quint64>());
	return { ids.begin(), ids.end() };
}

[[nodiscard]] QString Write(const std::set<quint64> &ids) {
	auto result = QStringList();
	for (const auto id : ids) result.push_back(QString::number(id));
	return result.join(u',');
}

void SetChatHidden(not_null<PeerData*> peer, bool hidden) {
	auto &options = ForAccount(&peer->session());
	auto ids = Ids(options.Get(kHiddenChatIds));
	if (hidden) ids.insert(peer->id.value);
	else ids.erase(peer->id.value);
	Expects(options.Set(kHiddenChatIds, Write(ids)));
	const auto history = peer->owner().history(peer);
	history->updateChatListExistence();
	if (hidden) {
		Core::App().notifications().clearFromHistory(history);
	}
}

void SetAccountHidden(not_null<Main::Account*> account, bool hidden) {
	if (!account->sessionExists()) return;
	auto ids = Ids(ForDevice().Get(kHiddenAccountIds));
	const auto id = account->session().uniqueId();
	if (hidden) ids.insert(id);
	else ids.erase(id);
	Expects(ForDevice().Set(kHiddenAccountIds, Write(ids)));
}

void AskUnlock(
		not_null<Window::SessionController*> controller,
		Fn<void()> done) {
	if (!Core::App().domain().local().hasLocalPasscode()) {
		controller->showToast(tr::lng_serein_hidden_requires_passcode(tr::now));
		return;
	}
	controller->show(Box([=](not_null<Ui::GenericBox*> box) {
		box->setTitle(tr::lng_serein_hidden_accounts());
		const auto row = box->addRow(object_ptr<Ui::RpWidget>(box));
		const auto field = Ui::CreateChild<Ui::PasswordInput>(
			row, st::defaultInputField, tr::lng_passcode_ph());
		row->resize(row->width(), field->height());
		row->widthValue() | rpl::on_next([=](int width) {
			field->resize(width, field->height());
		}, row->lifetime());
		box->setFocusCallback([=] { field->setFocusFast(); });
		const auto submit = [=] {
			if (!field->isEnabled() || field->getLastText().isEmpty()) {
				return;
			} else if (!passcodeCanTry()) {
				controller->showToast(tr::lng_flood_error(tr::now));
				return;
			}
			auto passcode = field->getLastText().toUtf8();
			const auto started = Core::App().domain().tryPasscode(passcode,
				[update = crl::guard(box, [=](bool correct) {
					field->setDisabled(false);
					if (!correct || Core::App().passcodeLocked()) {
						field->showError();
						field->selectAll();
						return;
					}
					field->clear();
					box->closeBox();
					crl::on_main(controller, done);
				})](bool correct) {
					cSetPasscodeBadTries(correct ? 0 : cPasscodeBadTries() + 1);
					cSetPasscodeLastTry(crl::now());
					update(correct);
				});
			OPENSSL_cleanse(passcode.data(), passcode.size());
			if (started) field->setDisabled(true);
		};
		QObject::connect(field, &Ui::MaskedInputField::submitted, field, submit);
		box->addButton(tr::lng_passcode_submit(), submit);
		box->addButton(tr::lng_cancel(), [=] { box->closeBox(); });
	}));
}

void ApplyAccountVisibility(Main::Domain &domain) {
	if (!domain.started()) return;
	if (AccountHidden(&domain.active())) domain.activate(VisibleAccount(&domain.active()));
	auto windows = std::vector<base::weak_ptr<Window::Controller>>();
	for (const auto &[index, account] : domain.accounts()) {
		if (!AccountHidden(account.get())) continue;
		const auto session = &account->session();
		Core::App().notifications().clearFromSession(session);
		for (const auto window : session->windows()) {
			windows.push_back(base::make_weak(&window->window()));
		}
	}
	for (const auto &weak : windows) {
		if (const auto window = weak.get()) {
			const auto current = window->sessionController();
			if (current && AccountHidden(&current->session().account())) window->close();
		}
	}
	domain.notifyAccountVisibilityChanged();
}

void HideCurrentAccount(not_null<Window::SessionController*> controller) {
	auto &domain = controller->session().domain();
	const auto account = &controller->session().account();
	auto other = (Main::Account*)nullptr;
	for (const auto &[index, candidate] : domain.accounts()) {
		if (candidate.get() != account && candidate->sessionExists() && !AccountHidden(candidate.get())) {
			other = candidate.get();
			break;
		}
	}
	if (!other || !domain.local().hasLocalPasscode()) {
		controller->showToast(tr::lng_serein_hide_account_unavailable(tr::now));
		return;
	}
	SetAccountHidden(account, true);
	ApplyAccountVisibility(domain);
}

void AccountsBox(not_null<Ui::GenericBox*> box, not_null<Window::SessionController*> controller) {
	box->setTitle(tr::lng_serein_hidden_accounts());
	box->addRow(object_ptr<Ui::FlatLabel>(box, tr::lng_serein_hidden_accounts_about(), st::boxLabel));
	if (ForDevice().Get(kHiddenAccountsEnabled)) {
		const auto hide = box->addRow(object_ptr<Ui::SettingsButton>(
			box, tr::lng_serein_hide_current_account(), st::settingsButtonNoIcon));
		hide->setClickedCallback([=] {
			box->closeBox();
			HideCurrentAccount(controller);
		});
	}
	for (const auto &[index, account] : controller->session().domain().accounts()) {
		if (!AccountHidden(account.get())) continue;
		const auto raw = account.get();
		const auto button = box->addRow(object_ptr<Ui::SettingsButton>(
			box, rpl::single(raw->session().user()->name()), st::settingsButtonNoIcon));
		button->setClickedCallback(crl::guard(raw, [=] {
			SetAccountHidden(raw, false);
			raw->domain().notifyAccountVisibilityChanged();
			box->closeBox();
			raw->domain().activate(raw);
		}));
	}
	box->addButton(tr::lng_close(), [=] { box->closeBox(); });
}

} // namespace

void WatchHiddenChats(not_null<Main::Session*> session) {
	const auto refresh = [=] {
		const auto update = [=](not_null<PeerData*> peer) {
			if (const auto history = session->data().historyLoaded(peer->id)) history->updateChatListExistence();
		};
		session->data().enumerateUsers(update);
		session->data().enumerateGroups(update);
		session->data().enumerateBroadcasts(update);
	};
	ForDevice().changes() | rpl::filter([](std::string_view key) {
		return key == kHiddenChatsEnabled.key;
	}) | rpl::on_next(refresh, session->lifetime());
}

bool ChatHidden(not_null<const PeerData*> peer) {
	return ForDevice().Get(kHiddenChatsEnabled)
		&& Ids(ForAccount(&peer->session()).Get(kHiddenChatIds)).contains(peer->id.value);
}

bool AccountHidden(not_null<Main::Account*> account) {
	return account->sessionExists()
		&& Core::App().domain().local().hasLocalPasscode()
		&& Ids(ForDevice().Get(kHiddenAccountIds)).contains(account->session().uniqueId());
}

bool HiddenAccountsAvailable() {
	return ForDevice().Get(kHiddenAccountsEnabled)
		|| ranges::any_of(Core::App().domain().accounts(), [](const auto &entry) {
			return AccountHidden(entry.account.get());
		});
}

not_null<Main::Account*> VisibleAccount(not_null<Main::Account*> account) {
	if (!AccountHidden(account)) return account;
	auto login = (Main::Account*)nullptr;
	for (const auto &[index, other] : account->domain().accounts()) {
		if (!AccountHidden(other.get())) {
			if (other->sessionExists()) return other.get();
			login = other.get();
		}
	}
	if (login) return login;
	SetAccountHidden(account, false);
	return account;
}

void ShowHiddenChats(not_null<Window::SessionController*> controller) {
	controller->show(Box([=](not_null<Ui::GenericBox*> box) {
		box->setTitle(tr::lng_serein_hidden_chats());
		const auto session = &controller->session();
		for (const auto id : Ids(ForAccount(session).Get(kHiddenChatIds))) {
			const auto peer = session->data().peer(PeerId(PeerIdHelper(id)));
			const auto button = box->addRow(object_ptr<Ui::SettingsButton>(
				box, rpl::single(peer->name()), st::settingsButtonNoIcon));
			button->setClickedCallback([=] {
				SetChatHidden(peer, false);
				box->closeBox();
				controller->showPeerHistory(peer);
			});
		}
		box->addButton(tr::lng_close(), [=] { box->closeBox(); });
	}));
}

void ShowHiddenAccounts(not_null<Window::SessionController*> controller) {
	if (!HiddenAccountsAvailable()) return;
	AskUnlock(controller, crl::guard(controller, [=] { controller->show(Box(AccountsBox, controller)); }));
}

void AddHideChatAction(
		const Ui::Menu::MenuCallback &addAction,
		not_null<Window::SessionController*> controller,
		not_null<PeerData*> peer) {
	if (!ForDevice().Get(kHiddenChatsEnabled) || peer->isSelf()) return;
	const auto hidden = ChatHidden(peer);
	addAction(hidden ? tr::lng_serein_restore_chat(tr::now) : tr::lng_serein_hide_chat(tr::now), [=] {
		SetChatHidden(peer, !hidden);
	}, &st::menuIconArchive);
}

} // namespace Serein::Privacy
