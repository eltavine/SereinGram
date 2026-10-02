#include "serein/features/stories/audience.h"

#include "apiwrap.h"
#include "boxes/peer_list_controllers.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "data/data_user.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "main/session/session_show.h"
#include "mtproto/mtproto_response.h"
#include "settings/settings_common.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/checkbox.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"
#include "styles/style_settings.h"

namespace Serein::Stories {
namespace {

using Users = std::vector<not_null<UserData*>>;

class PeoplePicker final : public ContactsBoxController {
public:
	PeoplePicker(
		not_null<Main::Session*> session,
		rpl::producer<QString> title,
		Users selected);

	void rowClicked(not_null<PeerListRow*> row) override;

protected:
	void prepareViewHook() override;
	std::unique_ptr<PeerListRow> createRow(
		not_null<UserData*> user) override;

private:
	rpl::producer<QString> _title;
	Users _selected;

};

PeoplePicker::PeoplePicker(
	not_null<Main::Session*> session,
	rpl::producer<QString> title,
	Users selected)
: ContactsBoxController(
	session,
	std::make_unique<PeerListGlobalSearchController>(session))
, _title(std::move(title))
, _selected(std::move(selected)) {
}

void PeoplePicker::rowClicked(not_null<PeerListRow*> row) {
	delegate()->peerListSetRowChecked(row, !row->checked());
}

void PeoplePicker::prepareViewHook() {
	delegate()->peerListSetTitle(std::move(_title));
	delegate()->peerListAddSelectedPeers(base::take(_selected));
}

std::unique_ptr<PeerListRow> PeoplePicker::createRow(
		not_null<UserData*> user) {
	if (user->isSelf() || user->isBot() || user->isInaccessible()) {
		return nullptr;
	}
	return ContactsBoxController::createRow(user);
}

void ChoosePeople(
		not_null<Window::SessionController*> controller,
		rpl::producer<QString> title,
		Users selected,
		Fn<void(Users)> done) {
	auto picker = std::make_unique<PeoplePicker>(
		&controller->session(),
		std::move(title),
		std::move(selected));
	auto init = [=](not_null<PeerListBox*> box) {
		box->addButton(tr::lng_settings_save(), [=] {
			auto result = Users();
			for (const auto &peer : box->collectSelectedRows()) {
				if (const auto user = peer->asUser()) {
					result.push_back(user);
				}
			}
			box->closeBox();
			done(std::move(result));
		});
		box->addButton(tr::lng_cancel(), [=] { box->closeBox(); });
	};
	controller->show(Box<PeerListBox>(std::move(picker), std::move(init)));
}

void SaveCloseFriends(
		not_null<Window::SessionController*> controller,
		const Users &users) {
	auto ids = QVector<MTPlong>();
	ids.reserve(int(users.size()));
	for (const auto &user : users) {
		ids.push_back(MTP_long(peerToUser(user->id).bare));
	}
	controller->session().api().request(MTPcontacts_EditCloseFriends(
		MTP_vector<MTPlong>(std::move(ids))
	)).done(crl::guard(controller, [=] {
		controller->showToast(
			tr::lng_serein_story_close_friends_saved(tr::now));
	})).fail(crl::guard(controller, [=](const MTP::Error &error) {
		MTP::ShowErrorFallback(controller->uiShow(), error);
	})).send();
}

void EditCloseFriends(not_null<Window::SessionController*> controller) {
	const auto session = &controller->session();
	session->api().request(MTPcontacts_GetContacts(
		MTP_long(0)
	)).done(crl::guard(controller, [=](
			const MTPcontacts_Contacts &result) {
		auto selected = Users();
		result.match([&](const MTPDcontacts_contacts &data) {
			session->data().processUsers(data.vusers());
			for (const auto &user : data.vusers().v) {
				user.match([&](const MTPDuser &fields) {
					if (fields.is_close_friend()) {
						selected.push_back(
							session->data().user(UserId(fields.vid().v)));
					}
				}, [](const MTPDuserEmpty &) {
				});
			}
		}, [](const MTPDcontacts_contactsNotModified &) {
		});
		ChoosePeople(
			controller,
			tr::lng_serein_story_edit_close_friends(),
			std::move(selected),
			[=](Users users) { SaveCloseFriends(controller, users); });
	})).fail(crl::guard(controller, [=](const MTP::Error &error) {
		MTP::ShowErrorFallback(controller->uiShow(), error);
	})).send();
}

[[nodiscard]] const Users &PeopleOf(const AudienceValue &value) {
	return TakesSelection(value.audience) ? value.selected : value.excluded;
}

[[nodiscard]] QString PeopleText(Audience audience) {
	if (TakesSelection(audience)) {
		return tr::lng_serein_story_choose_people(tr::now);
	} else if (TakesExclusions(audience)) {
		return tr::lng_serein_story_exclude(tr::now);
	}
	return tr::lng_serein_story_edit_close_friends(tr::now);
}

[[nodiscard]] QString PeopleLabel(const AudienceValue &value) {
	const auto &users = PeopleOf(value);
	if (value.audience == Audience::CloseFriends || users.empty()) {
		return QString();
	}
	return tr::lng_serein_story_people_count(
		tr::now,
		lt_number,
		QString::number(int(users.size())));
}

} // namespace

std::vector<AudienceRule> AudienceRulesFor(const AudienceValue &value) {
	auto ids = std::vector<std::uint64_t>();
	for (const auto &user : PeopleOf(value)) {
		ids.push_back(peerToUser(user->id).bare);
	}
	return AudienceRules(value.audience, ids);
}

void AddAudienceSection(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*> controller,
		not_null<rpl::variable<AudienceValue>*> value) {
	Ui::AddSubsectionTitle(container, tr::lng_serein_story_audience());
	const auto group = std::make_shared<Ui::RadiobuttonGroup>(
		int(value->current().audience));
	for (const auto &[audience, label] : {
		std::pair(Audience::Everyone, tr::lng_edit_privacy_everyone(tr::now)),
		std::pair(Audience::Contacts, tr::lng_edit_privacy_contacts(tr::now)),
		std::pair(
			Audience::CloseFriends,
			tr::lng_edit_privacy_close_friends(tr::now)),
		std::pair(
			Audience::Selected,
			tr::lng_serein_story_audience_selected(tr::now)),
	}) {
		container->add(
			object_ptr<Ui::Radiobutton>(
				container,
				group,
				int(audience),
				label,
				st::settingsSendType),
			st::settingsSendTypePadding);
	}
	group->setChangedCallback([=](int chosen) {
		auto updated = value->current();
		updated.audience = Audience(chosen);
		*value = std::move(updated);
	});
	const auto people = Settings::AddButtonWithLabel(
		container,
		value->value() | rpl::map([](const AudienceValue &current) {
			return PeopleText(current.audience);
		}),
		value->value() | rpl::map(PeopleLabel),
		st::settingsButtonNoIcon);
	people->setClickedCallback([=] {
		const auto current = value->current();
		if (current.audience == Audience::CloseFriends) {
			EditCloseFriends(controller);
			return;
		}
		const auto selection = TakesSelection(current.audience);
		ChoosePeople(
			controller,
			selection
				? tr::lng_serein_story_choose_people()
				: tr::lng_serein_story_exclude(),
			PeopleOf(current),
			crl::guard(container, [=](Users users) {
				auto updated = value->current();
				(selection ? updated.selected : updated.excluded)
					= std::move(users);
				*value = std::move(updated);
			}));
	});
}

} // namespace Serein::Stories
