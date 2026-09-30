#include "serein/menu/rating.h"

#include "serein/hooks/menu/actions.h"
#include "serein/menu/model.h"
#include "api/api_common.h"
#include "apiwrap.h"
#include "data/data_chat_participant_status.h"
#include "data/data_forum_topic.h"
#include "data/data_session.h"
#include "data/data_thread.h"
#include "history/history.h"
#include "history/history_item.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "ui/widgets/menu/menu_action.h"
#include "ui/widgets/menu/menu.h"
#include "ui/widgets/popup_menu.h"
#include "window/window_session_controller.h"
#include "styles/style_menu_icons.h"

namespace Serein::Menu {
namespace {

constexpr auto kActionIdProperty = "sereinMenuActionId";
constexpr auto kLabelLength = 32;

[[nodiscard]] Data::Thread *ReplyThread(not_null<HistoryItem*> item) {
	if (item->isService() || item->isLocal() || item->id <= 0) {
		return nullptr;
	}
	const auto thread = item->topic()
		? static_cast<Data::Thread*>(item->topic())
		: static_cast<Data::Thread*>(item->history());
	return Data::CanSendTexts(thread) ? thread : nullptr;
}

void SendRating(
		not_null<Window::SessionController*> controller,
		FullMsgId itemId,
		const QString &text) {
	const auto item = controller->session().data().message(itemId);
	const auto thread = item ? ReplyThread(item) : nullptr;
	if (!thread) {
		return;
	}
	auto action = Api::SendAction(thread);
	action.replyTo.messageId = itemId;
	action.clearDraft = false;
	auto message = Api::MessageToSend(std::move(action));
	message.textWithTags = { text };
	controller->session().api().sendMessage(std::move(message));
}

[[nodiscard]] int InsertPosition(not_null<Ui::PopupMenu*> menu) {
	for (auto index = 0; index != int(menu->actions().size()); ++index) {
		const auto value = menu->actions()[index]->property(kActionIdProperty);
		if (value.isValid() && value.toInt() == int(ActionId::Reply)) {
			return index + 1;
		}
	}
	return DeleteActionIndex(menu);
}

[[nodiscard]] QString Label(const QString &text) {
	const auto shown = (text.size() > kLabelLength)
		? (text.left(kLabelLength - 1) + QChar(0x2026))
		: text;
	return tr::lng_serein_menu_quick_rating_send(tr::now, lt_text, shown);
}

} // namespace

void InsertQuickRatingActions(
		Ui::PopupMenu *menu,
		HistoryItem *item,
		Window::SessionController *controller) {
	if (!menu || !item || !controller || !ReplyThread(item)) {
		return;
	}
	const auto itemId = item->fullId();
	auto position = InsertPosition(menu);
	const auto ratings = {
		std::pair{ ActionId::QuickRatingFirst, &kQuickRatingFirst },
		std::pair{ ActionId::QuickRatingSecond, &kQuickRatingSecond },
	};
	for (const auto &[id, option] : ratings) {
		const auto text = ForDevice().Get(*option).trimmed();
		if (text.isEmpty()) {
			continue;
		}
		const auto callback = crl::guard(controller, [=] {
			SendRating(controller, itemId, text);
		});
		const auto action = Ui::Menu::CreateAction(
			menu,
			Label(text),
			callback);
		const auto icon = (id == ActionId::QuickRatingFirst)
			? &st::menuIconLike
			: &st::menuIconReply;
		auto widget = base::make_unique_q<Ui::Menu::Action>(
			menu->menu(), menu->menu()->st(), action, icon, icon);
		Tag(menu->insertAction(position++, std::move(widget)), id);
	}
}

} // namespace Serein::Menu
