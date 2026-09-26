#include "nagram/menu/repeat.h"

#include "nagram/menu/actions.h"
#include "api/api_common.h"
#include "api/api_sending.h"
#include "apiwrap.h"
#include "data/data_chat_participant_status.h"
#include "data/data_forum_topic.h"
#include "data/data_session.h"
#include "history/history.h"
#include "history/history_item.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "ui/boxes/confirm_box.h"
#include "ui/text/text_utilities.h"
#include "ui/widgets/menu/menu_action.h"
#include "ui/widgets/menu/menu.h"
#include "ui/widgets/popup_menu.h"
#include "window/window_session_controller.h"
#include "styles/style_menu_icons.h"

namespace Nagram::Menu {
namespace {

constexpr auto kActionIdProperty = "nagramMenuActionId";

bool Available(HistoryItem *item) {
	if (!item || !item->allowsForward() || item->isService()
		|| item->isLocal() || item->id <= 0) {
		return false;
	}
	const auto target = item->topic()
		? static_cast<Data::Thread*>(item->topic())
		: static_cast<Data::Thread*>(item->history());
	return Data::CanSendAnything(target);
}

void SendRepeat(
		not_null<Window::SessionController*> controller,
		FullMsgId itemId,
		ActionId id) {
	const auto item = controller->session().data().message(itemId);
	if (!Available(item)) {
		return;
	}
	const auto target = item->topic()
		? static_cast<Data::Thread*>(item->topic())
		: static_cast<Data::Thread*>(item->history());
	const auto history = item->history();
	if (id == ActionId::RepeatAsCopy) {
		const auto media = item->media();
		const auto &original = item->originalText();
		auto action = Api::SendAction(target);
		action.clearDraft = false;
		auto message = Api::MessageToSend(std::move(action));
		message.textWithTags = { original.text,
			TextUtilities::ConvertEntitiesToTextTags(original.entities) };
		if (const auto photo = media ? media->photo() : nullptr) {
			Api::SendExistingPhoto(std::move(message), photo);
		} else if (const auto document = media ? media->document() : nullptr) {
			Api::SendExistingDocument(std::move(message), document);
		} else if (!original.text.isEmpty()) {
			history->session().api().sendMessage(std::move(message));
		}
		return;
	}
	auto draft = Data::ForwardDraft{
		.ids = history->owner().itemOrItsGroup(item),
		.options = (id == ActionId::Repeat)
			? Data::ForwardOptions::PreserveInfo
			: Data::ForwardOptions::NoSenderNames,
	};
	auto resolved = history->resolveForwardDraft(draft);
	if (resolved.items.empty()) {
		return;
	}
	auto action = Api::SendAction(target);
	action.clearDraft = false;
	action.generateLocal = false;
	history->session().api().forwardMessages(
		std::move(resolved), action);
}

int InsertPosition(not_null<Ui::PopupMenu*> menu) {
	for (auto index = int(menu->actions().size()); index != 0;) {
		--index;
		const auto value = menu->actions()[index]->property(kActionIdProperty);
		if (value.isValid()
			&& value.toInt() == static_cast<int>(ActionId::Forward)) {
			return index + 1;
		}
	}
	for (auto index = 0; index != int(menu->actions().size()); ++index) {
		const auto value = menu->actions()[index]->property(kActionIdProperty);
		if (value.isValid()
			&& value.toInt() == static_cast<int>(ActionId::Delete)) {
			return index;
		}
	}
	return int(menu->actions().size());
}

void Insert(
		not_null<Ui::PopupMenu*> menu,
		int position,
		not_null<Window::SessionController*> controller,
		FullMsgId itemId,
		ActionId id,
		const QString &text) {
	const auto callback = crl::guard(controller, [=] {
		if (id != ActionId::ForwardWithoutQuote
			&& ForDevice().Get(kConfirmRepeat)) {
			const auto weak = base::make_weak(controller);
			controller->show(Ui::MakeConfirmBox({
				.text = tr::lng_nagram_menu_repeat_confirm_text(),
				.confirmed = [=](Fn<void()> &&close) {
					close();
					if (const auto strong = weak.get()) {
						SendRepeat(strong, itemId, id);
					}
				},
			}));
		} else {
			SendRepeat(controller, itemId, id);
		}
	});
	const auto action = Ui::Menu::CreateAction(menu, text, callback);
	const auto icon = (id == ActionId::ForwardWithoutQuote)
		? &st::menuIconForward : &st::menuIconRepeat;
	auto widget = base::make_unique_q<Ui::Menu::Action>(
		menu->menu(), menu->menu()->st(), action, icon, icon);
	Tag(menu->insertAction(position, std::move(widget)), id);
}

} // namespace

void InsertRepeatActions(
		Ui::PopupMenu *menu,
		HistoryItem *item,
		Window::SessionController *controller) {
	if (!Available(item) || !menu || !controller) {
		return;
	}
	const auto itemId = item->fullId();
	auto position = InsertPosition(menu);
	Insert(menu, position++, controller, itemId, ActionId::Repeat,
		tr::lng_nagram_menu_repeat(tr::now));
	Insert(menu, position++, controller, itemId, ActionId::RepeatAsCopy,
		tr::lng_nagram_menu_repeat_as_copy(tr::now));
	Insert(menu, position, controller, itemId, ActionId::ForwardWithoutQuote,
		tr::lng_nagram_menu_forward_without_quote(tr::now));
}

} // namespace Nagram::Menu
