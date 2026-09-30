#include "serein/admin/shortcuts.h"

#include "serein/chats/options.h"
#include "boxes/peers/edit_participants_box.h"
#include "data/data_channel.h"
#include "data/data_chat.h"
#include "history/admin_log/history_admin_log_section.h"
#include "lang/lang_keys.h"
#include "ui/widgets/menu/menu_add_action_callback.h"
#include "window/window_session_controller.h"
#include "styles/style_menu_icons.h"

namespace Serein::Admin {

void FillManagementShortcuts(
		const Ui::Menu::MenuCallback &addAction,
		gsl::not_null<Window::SessionController*> controller,
		gsl::not_null<PeerData*> peer) {
	if (!ForDevice().Get(Chats::kManagementShortcuts)
		|| (!peer->isChat() && !peer->isMegagroup())) {
		return;
	}
	using Role = ParticipantsBoxController::Role;
	const auto chat = peer->asChat();
	const auto channel = peer->asChannel();
	const auto open = [=](Role role) {
		return [=] {
			ParticipantsBoxController::Start(controller, peer, role);
		};
	};
	if (channel ? channel->canViewMembers() : chat->amIn()) {
		addAction(
			tr::lng_serein_manage_members(tr::now),
			open(Role::Members),
			&st::menuIconGroups);
	}
	if (channel ? channel->canViewAdmins() : chat->amIn()) {
		addAction(
			tr::lng_serein_manage_admins(tr::now),
			open(Role::Admins),
			&st::menuIconAdmin);
	}
	if (channel && channel->isGigagroup()) {
		addAction(
			tr::lng_serein_manage_removed(tr::now),
			open(Role::Kicked),
			&st::menuIconRemovedUsers);
	}
	if (channel && (channel->hasAdminRights() || channel->amCreator())) {
		addAction(tr::lng_serein_manage_recent_actions(tr::now), [=] {
			controller->showSection(
				std::make_shared<AdminLog::SectionMemento>(channel));
		}, &st::menuIconGroupLog);
	}
}

} // namespace Serein::Admin
