#include "serein/hooks/peer_menu.h"

#include "serein/admin/delete_mine.h"
#include "serein/admin/shortcuts.h"
#include "serein/admin/upgrade.h"
#include "serein/chats/local_pins.h"
#include "serein/chats/options.h"
#include "serein/chats/quick_actions.h"
#include "serein/features/history/viewer.h"
#include "serein/services/summary.h"
#include "serein/hooks/privacy/alias.h"
#include "serein/privacy/options.h"
#include "data/data_forum_topic.h"
#include "data/data_peer.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
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
	if (ForDevice().Get(Chats::kChatQuickActions)) {
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
	}
	if (!topic) {
		Chats::FillQuickActions(addAction, controller, peer);
	}
	if (!topic && CanSummarizeChats()) {
		addAction(tr::lng_serein_summary_action(tr::now), [=] {
			ShowChatSummary(controller, peer);
		}, &st::menuIconTranslate);
	}
	if (!topic && ForDevice().Get(Chats::kLocalPinning)) {
		const auto id = SerializePeerId(peer->id);
		const auto pins = Chats::ParseLocalPins(
			ForAccount(&controller->session()).Get(Chats::kLocalPins));
		const auto pinned = std::find(pins.begin(), pins.end(), id)
			!= pins.end();
		const auto full = int(pins.size()) >= Chats::kLocalPinsLimit;
		addAction(pinned
			? tr::lng_serein_local_unpin(tr::now)
			: tr::lng_serein_local_pin(tr::now), [=] {
			if (!pinned && full) {
				controller->uiShow()->showToast(
					tr::lng_serein_local_pin_limit(tr::now));
				return;
			}
			auto &options = ForAccount(&controller->session());
			Expects(options.Set(
				Chats::kLocalPins,
				Chats::ToggleLocalPin(options.Get(Chats::kLocalPins), id)));
		}, pinned ? &st::menuIconUnpin : &st::menuIconPin);
	}
	if (!topic
		&& HistoryFeature::HasDeletedMessages(&controller->session(), peer)) {
		addAction(tr::lng_serein_menu_deleted_messages(tr::now), [=] {
			HistoryFeature::ShowDeletedMessages(controller, peer);
		}, &st::menuIconInfo);
	}
	if (!topic
		&& ForDevice().Get(Chats::kManagementShortcuts)
		&& Admin::CanDeleteMyMessages(peer)) {
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
	if (ForDevice().Get(Privacy::kLocalNames)) {
		addAction(tr::lng_serein_peer_alias(tr::now), [=] {
			Privacy::ShowAlias(show, peer);
		}, &st::menuIconEdit);
	}
	if (ForDevice().Get(Chats::kManagementShortcuts)
		&& Admin::CanUpgradeToSupergroup(peer)) {
		addAction(tr::lng_serein_upgrade_supergroup(tr::now), [=] {
			Admin::ConfirmUpgradeToSupergroup(controller, peer);
		}, &st::menuIconGroups);
	}
	Admin::FillManagementShortcuts(addAction, controller, peer);
}

} // namespace Serein::Hooks
