#include "serein/chats/cleanup.h"

#include "serein/features/cleanup/model/candidates.h"
#include "apiwrap.h"
#include "base/timer.h"
#include "base/unixtime.h"
#include "data/data_channel.h"
#include "data/data_chat.h"
#include "data/data_session.h"
#include "data/data_user.h"
#include "dialogs/dialogs_indexed_list.h"
#include "dialogs/dialogs_main_list.h"
#include "dialogs/dialogs_row.h"
#include "history/history.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "ui/boxes/confirm_box.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/checkbox.h"
#include "ui/widgets/labels.h"
#include "window/window_session_controller.h"

#include "styles/style_layers.h"

#include <array>
#include <deque>

namespace Serein::Chats {
namespace {

using Cleanup::Category;

constexpr auto kActionInterval = crl::time(1000);

struct Candidate {
	not_null<History*> history;
	Category category;
	Ui::Checkbox *check = nullptr;
};

[[nodiscard]] Cleanup::ChatFacts Facts(not_null<History*> history) {
	const auto peer = history->peer;
	const auto user = peer->asUser();
	const auto chat = peer->asChat();
	const auto channel = peer->asChannel();
	return {
		.user = (user != nullptr),
		.deleted = user && user->isInaccessible(),
		.bot = user && user->isBot(),
		.kept = peer->isSelf()
			|| peer->isServiceUser()
			|| history->isPinnedDialog(0)
			|| (chat && chat->amCreator())
			|| (channel && channel->amCreator()),
		.lastActivity = history->chatListTimeId(),
	};
}

[[nodiscard]] std::vector<Candidate> Collect(
		not_null<Main::Session*> session) {
	auto result = std::vector<Candidate>();
	const auto now = base::unixtime::now();
	for (const auto &row : session->data().chatsList()->indexed()->all()) {
		if (const auto history = row->history()) {
			if (const auto category = Cleanup::Classify(Facts(history), now)) {
				result.push_back({ history, *category });
			}
		}
	}
	return result;
}

[[nodiscard]] tr::phrase<> Title(Category category) {
	switch (category) {
	case Category::DeletedAccount: return tr::lng_serein_cleanup_deleted;
	case Category::UnusedBot: return tr::lng_serein_cleanup_bots;
	case Category::InactiveChat: return tr::lng_serein_cleanup_inactive;
	}
	Unexpected("Category in Chats::Title.");
}

void Apply(not_null<History*> history, bool remove) {
	const auto peer = history->peer;
	if (!remove) {
		peer->session().api().toggleHistoryArchived(history, true, [] {});
	} else if (const auto channel = peer->asChannel()) {
		peer->session().api().leaveChannel(channel);
	} else {
		peer->session().api().deleteConversation(peer, false);
	}
}

void CleanupBox(
		not_null<Ui::GenericBox*> box,
		not_null<Window::SessionController*> controller) {
	struct State {
		std::vector<Candidate> candidates;
		std::deque<not_null<History*>> queue;
		base::Timer timer;
		int total = 0;
		bool remove = false;
	};
	box->setTitle(tr::lng_serein_cleanup());
	box->addRow(object_ptr<Ui::FlatLabel>(
		box,
		tr::lng_serein_cleanup_about(),
		st::boxDividerLabel));
	const auto state = box->lifetime().make_state<State>();
	state->candidates = Collect(&controller->session());
	if (state->candidates.empty()) {
		box->addRow(object_ptr<Ui::FlatLabel>(
			box,
			tr::lng_serein_cleanup_empty(),
			st::boxLabel));
		box->addButton(tr::lng_close(), [=] { box->closeBox(); });
		return;
	}
	for (const auto category : std::array{
		Category::DeletedAccount,
		Category::UnusedBot,
		Category::InactiveChat,
	}) {
		auto header = false;
		for (auto &candidate : state->candidates) {
			if (candidate.category != category) {
				continue;
			} else if (!header) {
				header = true;
				box->addRow(object_ptr<Ui::FlatLabel>(
					box,
					Title(category)(),
					st::boxLabel));
			}
			candidate.check = box->addRow(object_ptr<Ui::Checkbox>(
				box,
				candidate.history->peer->name(),
				category == Category::DeletedAccount));
		}
	}
	const auto progress = box->addRow(object_ptr<Ui::FlatLabel>(
		box,
		QString(),
		st::boxDividerLabel));
	state->timer.setCallback([=] {
		if (state->queue.empty()) {
			state->timer.cancel();
			return;
		}
		Apply(state->queue.front(), state->remove);
		state->queue.pop_front();
		progress->setText(tr::lng_serein_cleanup_progress(
			tr::now,
			lt_done,
			QString::number(state->total - int(state->queue.size())),
			lt_total,
			QString::number(state->total)));
	});
	const auto run = [=](bool remove) {
		if (!state->queue.empty()) {
			box->showToast(tr::lng_serein_cleanup_busy(tr::now));
			return;
		}
		for (const auto &candidate : state->candidates) {
			const auto check = candidate.check;
			if (check && check->checked() && !check->isDisabled()) {
				check->setDisabled(true);
				state->queue.push_back(candidate.history);
			}
		}
		if (state->queue.empty()) {
			box->showToast(tr::lng_serein_cleanup_none_selected(tr::now));
			return;
		}
		state->total = int(state->queue.size());
		state->remove = remove;
		state->timer.callEach(kActionInterval);
	};
	box->addButton(tr::lng_serein_cleanup_archive(), [=] { run(false); });
	box->addButton(tr::lng_serein_cleanup_delete(), [=] {
		controller->show(Ui::MakeConfirmBox({
			.text = tr::lng_serein_cleanup_delete_sure(),
			.confirmed = [=](Fn<void()> close) {
				close();
				run(true);
			},
			.confirmText = tr::lng_serein_cleanup_delete(),
			.confirmStyle = &st::attentionBoxButton,
		}));
	});
	box->addButton(tr::lng_close(), [=] { box->closeBox(); });
}

} // namespace

void ShowChatCleanup(not_null<Window::SessionController*> controller) {
	controller->show(Box(CleanupBox, controller));
}

} // namespace Serein::Chats
