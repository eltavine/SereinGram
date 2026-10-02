#include "serein/hooks/peer_menu.h"

#include "serein/admin/delete_mine.h"
#include "serein/admin/shortcuts.h"
#include "serein/admin/upgrade.h"
#include "serein/chats/local_pins.h"
#include "serein/chats/options.h"
#include "serein/chats/quick_actions.h"
#include "serein/features/history/viewer.h"
#include "serein/features/stories/composer.h"
#include "serein/filters/model.h"
#include "serein/filters/reveal.h"
#include "serein/services/summary.h"
#include "serein/hooks/privacy/alias.h"
#include "serein/hooks/ghost.h"
#include "serein/features/ghost/model/exceptions.h"
#include "serein/schema/gen/settings/ghost.h"
#include "serein/schema/gen/settings/media.h"
#include "serein/privacy/options.h"
#include "data/data_forum_topic.h"
#include "data/data_histories.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "ui/widgets/menu/menu_add_action_callback.h"
#include "window/window_session_controller.h"
#include "styles/style_menu_icons.h"

namespace Serein::Hooks {

namespace {

void FillReadExceptionAction(
		const Ui::Menu::MenuCallback &addAction,
		gsl::not_null<Window::SessionController*> controller,
		gsl::not_null<PeerData*> peer) {
	const auto session = &controller->session();
	const auto excepted = Serein::Ghost::HasException(
		ForAccount(session).Get(Serein::Ghost::kReadReceiptExceptions),
		peer->id.value);
	if (!excepted && AllowReadReceipt(session)) {
		return;
	}
	addAction(excepted
		? tr::lng_serein_ghost_read_here_off(tr::now)
		: tr::lng_serein_ghost_read_here(tr::now), [=] {
		auto &options = ForAccount(session);
		Expects(options.Set(
			Serein::Ghost::kReadReceiptExceptions,
			Serein::Ghost::ToggleException(
				options.Get(Serein::Ghost::kReadReceiptExceptions),
				peer->id.value)));
		if (!excepted) {
			session->data().histories().readInbox(
				session->data().history(peer));
		}
	}, &st::menuIconMarkRead);
}

} // namespace

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
	if (!topic) {
		FillReadExceptionAction(addAction, controller, peer);
	}
	if (!topic
		&& ForDevice().Get(Serein::Media::kStoryPosting)
		&& peer->isChannel()
		&& peer->canPostStories()) {
		addAction(tr::lng_serein_story_post_channel(tr::now), [=] {
			Serein::Stories::StartPosting(controller->uiShow(), peer);
		}, &st::menuIconStoriesSavedSection);
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
	if (const auto filters = Serein::Filters::ReadRules(
			ForAccount(&controller->session()).Get(Serein::Filters::kRules))
		; !topic && filters && filters->enabled) {
		const auto history = controller->session().data().history(peer);
		const auto revealed = Serein::Filters::Revealed(history);
		addAction(revealed
			? tr::lng_serein_filter_hide_in_chat(tr::now)
			: tr::lng_serein_filter_show_in_chat(tr::now), [=] {
			Serein::Filters::ToggleRevealed(history);
		}, revealed ? &st::menuIconCaptionHide : &st::menuIconShowAll);
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
