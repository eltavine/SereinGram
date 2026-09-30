#include "serein/hooks/peer_menu.h"

#include "serein/admin/delete_mine.h"
#include "serein/admin/upgrade.h"
#include "serein/chats/quick_actions.h"
#include "serein/features/history/viewer.h"
#include "serein/hooks/privacy/alias.h"
#include "data/data_forum_topic.h"
#include "data/data_peer.h"
#include "lang/lang_keys.h"
#include "ui/widgets/menu/menu_add_action_callback.h"
#include "window/window_session_controller.h"
#include "styles/style_menu_icons.h"

namespace Serein::Hooks {

void FillHistoryMenu(
		const Ui::Menu::MenuCallback &addAction,
		gsl::not_null<Window::SessionController*> controller,
		gsl::not_null<PeerData*> peer,
		Data::ForumTopic *topic) {
	const auto weak = base::make_weak(topic);
	addAction(tr::lng_serein_jump_to_beginning(tr::now), [=] {
		if (!topic) {
			controller->showPeerHistory(
				peer,
				Window::SectionShow::Way::Forward,
				MsgId(1));
		} else if (const auto strong = weak.get()) {
			controller->showTopic(
				strong,
				strong->rootId(),
				Window::SectionShow::Way::Forward);
		}
	}, &st::menuIconShowInChat);
	if (!topic) {
		Chats::FillQuickActions(addAction, controller, peer);
	}
	if (!topic
		&& HistoryFeature::HasDeletedMessages(&controller->session(), peer)) {
		addAction(tr::lng_serein_menu_deleted_messages(tr::now), [=] {
			HistoryFeature::ShowDeletedMessages(controller, peer);
		}, &st::menuIconInfo);
	}
	if (!topic && Admin::CanDeleteMyMessages(peer)) {
		addAction({
			.text = tr::lng_serein_delete_mine(tr::now),
			.handler = [=] {
				Admin::ConfirmDeleteMyMessages(controller, peer);
			},
			.icon = &st::menuIconDeleteAttention,
			.isAttention = true,
		});
	}
}

void FillProfileMenu(
		const Ui::Menu::MenuCallback &addAction,
		gsl::not_null<Window::SessionController*> controller,
		gsl::not_null<PeerData*> peer,
		Data::ForumTopic *topic) {
	if (topic || peer->isSelf() || peer->migrateTo()) {
		return;
	}
	const auto show = controller->uiShow();
	addAction(tr::lng_serein_peer_alias(tr::now), [=] {
		Privacy::ShowAlias(show, peer);
	}, &st::menuIconEdit);
	if (Admin::CanUpgradeToSupergroup(peer)) {
		addAction(tr::lng_serein_upgrade_supergroup(tr::now), [=] {
			Admin::ConfirmUpgradeToSupergroup(controller, peer);
		}, &st::menuIconGroups);
	}
}

} // namespace Serein::Hooks
