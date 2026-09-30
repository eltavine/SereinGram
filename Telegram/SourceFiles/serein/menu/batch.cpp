#include "serein/menu/batch.h"

#include "serein/hooks/menu/actions.h"
#include "data/data_changes.h"
#include "data/data_chat_participant_status.h"
#include "data/data_drafts.h"
#include "data/data_forum_topic.h"
#include "data/data_session.h"
#include "data/data_user.h"
#include "history/history.h"
#include "history/history_item.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "apiwrap.h"
#include "mainwidget.h"
#include "storage/storage_account.h"
#include "ui/layers/generic_box.h"
#include "ui/text/text_utilities.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/checkbox.h"
#include "ui/widgets/fields/input_field.h"
#include "ui/widgets/labels.h"
#include "ui/widgets/menu/menu_action.h"
#include "ui/widgets/menu/menu.h"
#include "ui/widgets/popup_menu.h"
#include "window/window_session_controller.h"
#include "styles/style_layers.h"
#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"

namespace Serein::Menu {
namespace {

constexpr auto kMaximumMessages = 100;
constexpr auto kMaximumText = 32768;
constexpr auto kActionIdProperty = "sereinMenuActionId";

bool Available(HistoryItem *item) {
	return item && item->allowsForward() && !item->isTtlCoveredMedia()
		&& !item->isEphemeral() && !item->isMediaSensitive();
}

std::optional<TextWithEntities> Prepare(
		not_null<Main::Session*> session,
		MessageIdsList ids,
		bool reverse) {
	if (ids.size() < 2 || ids.size() > kMaximumMessages) {
		return std::nullopt;
	}
	if (base::flat_set<FullMsgId>(ids.begin(), ids.end()).size() != ids.size()) {
		return std::nullopt;
	}
	if (reverse) {
		ranges::reverse(ids);
	}
	auto text = TextWithEntities();
	for (const auto id : ids) {
		const auto item = session->data().message(id);
		if (!Available(item)) {
			return std::nullopt;
		}
		if (!text.empty()) {
			text.append(u"\n\n"_q);
		}
		text.append(item->clipboardText().rich);
		if (text.text.size() > kMaximumText) {
			return std::nullopt;
		}
	}
	return text;
}

bool DraftOccupied(not_null<Data::Thread*> target) {
	const auto history = target->owningHistory();
	const auto topic = target->topicRootId();
	const auto monoforum = target->monoforumPeerId();
	auto &local = target->session().local();
	local.readDraftsWithCursors(history);
	return !Data::DraftIsNull(history->localDraft(topic, monoforum))
		|| !Data::DraftIsNull(history->cloudDraft(topic, monoforum))
		|| history->localEditDraft(topic, monoforum)
		|| !history->forwardDraft(topic, monoforum).ids.empty();
}

void PlaceTextDraft(
		not_null<Window::SessionController*> controller,
		not_null<Data::Thread*> target,
		TextWithTags text) {
	const auto history = target->owningHistory();
	const auto topic = target->topicRootId();
	const auto monoforum = target->monoforumPeerId();
	history->setLocalDraft(std::make_unique<Data::Draft>(text, FullReplyTo{
		.topicRootId = topic,
		.monoforumPeerId = monoforum,
	}, SuggestOptions(), MessageCursor{
		int(text.text.size()), int(text.text.size()), Ui::kQFixedMax,
	}, Data::WebPageDraft()));
	controller->session().changes().entryUpdated(
		target, Data::EntryUpdate::Flag::LocalDraftSet);
	auto params = Window::SectionShow();
	params.reapplyLocalDraft = true;
	controller->showThread(target, ShowAtTheEndMsgId, params);
}

void BatchBox(
		not_null<Ui::GenericBox*> box,
		not_null<Window::SessionController*> controller,
		MessageIdsList ids,
		FullMsgId sourceId) {
	box->setTitle(tr::lng_serein_menu_batch());
	box->addRow(object_ptr<Ui::FlatLabel>(
		box, tr::lng_serein_menu_batch_preview(), st::boxLabel));
	const auto field = box->addRow(object_ptr<Ui::InputField>(
		box, st::defaultInputField, Ui::InputField::Mode::MultiLine));
	field->setMaxLength(kMaximumText);
	const auto reverse = box->addRow(object_ptr<Ui::Checkbox>(
		box, tr::lng_serein_menu_batch_reverse(tr::now), false));
	const auto omitNames = box->addRow(object_ptr<Ui::Checkbox>(
		box, tr::lng_serein_menu_batch_no_names(tr::now), false));
	const auto prepared = box->lifetime().make_state<std::optional<TextWithEntities>>();
	const auto rebuild = [=] {
		*prepared = Prepare(&controller->session(), ids, reverse->checked());
		field->setTextWithTags(*prepared
			? TextWithTags{ (*prepared)->text,
				TextUtilities::ConvertEntitiesToTextTags((*prepared)->entities) }
			: TextWithTags());
	};
	reverse->checkedChanges() | rpl::on_next(rebuild, box->lifetime());
	const auto validate = [=] {
		const auto current = Prepare(&controller->session(), ids, reverse->checked());
		if (!current || !*prepared || *current != **prepared) {
			box->showToast(tr::lng_serein_menu_batch_changed(tr::now));
			return false;
		}
		return true;
	};
	const auto draft = box->addRow(object_ptr<Ui::SettingsButton>(
		box, tr::lng_serein_menu_batch_draft(), st::settingsButtonNoIcon));
	draft->setClickedCallback(crl::guard(controller, [=] {
		if (!validate()) {
			return;
		}
		const auto source = controller->session().data().message(sourceId);
		if (!source) {
			box->showToast(tr::lng_serein_menu_batch_changed(tr::now));
			return;
		}
		const auto target = source->topic()
			? static_cast<Data::Thread*>(source->topic())
			: static_cast<Data::Thread*>(source->history());
		if (DraftOccupied(target)) {
			box->showToast(tr::lng_serein_menu_batch_draft_exists(tr::now));
			return;
		}
		if (!Data::CanSendTexts(target)) {
			box->showToast(tr::lng_serein_menu_batch_unavailable(tr::now));
			return;
		}
		const auto entered = field->getTextWithTags();
		if (entered.text.isEmpty()) {
			box->showToast(tr::lng_serein_menu_batch_no_text(tr::now));
			return;
		}
		const auto weakBox = base::make_weak(box);
		PlaceTextDraft(controller, target, entered);
		if (weakBox) {
			weakBox->closeBox();
		}
	}));
	const auto saved = box->addRow(object_ptr<Ui::SettingsButton>(
		box, tr::lng_serein_menu_batch_saved(), st::settingsButtonNoIcon));
	saved->setClickedCallback(crl::guard(controller, [=] {
		if (!validate()) {
			return;
		}
		const auto target = controller->session().data().history(
			controller->session().user());
		if (DraftOccupied(target)) {
			box->showToast(tr::lng_serein_menu_batch_draft_exists(tr::now));
			return;
		}
		auto ordered = ids;
		if (reverse->checked()) {
			ranges::reverse(ordered);
		}
		if (controller->content()->setForwardDraft(target, {
			.ids = std::move(ordered),
			.options = omitNames->checked()
				? Data::ForwardOptions::NoSenderNames
				: Data::ForwardOptions::PreserveInfo,
		})) {
			box->closeBox();
		} else {
			box->showToast(tr::lng_serein_menu_batch_unavailable(tr::now));
		}
	}));
	box->addButton(tr::lng_close(), [=] { box->closeBox(); });
	rebuild();
}

int EndPosition(not_null<Ui::PopupMenu*> menu) {
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
		ActionId id,
		const QString &text,
		Fn<void()> callback) {
	const auto action = Ui::Menu::CreateAction(menu, text, std::move(callback));
	const auto icon = (id == ActionId::Batch)
		? &st::menuIconCopy : &st::menuIconSelect;
	auto widget = base::make_unique_q<Ui::Menu::Action>(
		menu->menu(), menu->menu()->st(), action, icon, icon);
	Tag(menu->insertAction(position, std::move(widget)), id);
}

[[nodiscard]] MessageIdsList PinnedAmong(
		not_null<Main::Session*> session,
		const MessageIdsList &ids) {
	auto result = MessageIdsList();
	for (const auto &id : ids) {
		if (const auto item = session->data().message(id)) {
			if (item->isPinned() && item->canPin()) {
				result.push_back(id);
			}
		}
	}
	return result;
}

void UnpinSequentially(
		not_null<Main::Session*> session,
		MessageIdsList ids) {
	const auto unpin = [=](auto self, int index) -> void {
		for (auto i = index; i < int(ids.size()); ++i) {
			const auto item = session->data().message(ids[i]);
			if (!item || !item->isPinned() || !item->canPin()) {
				continue;
			}
			session->api().request(MTPmessages_UpdatePinnedMessage(
				MTP_flags(MTPmessages_UpdatePinnedMessage::Flag::f_unpin),
				item->history()->peer->input(),
				MTP_int(item->id)
			)).done([=](const MTPUpdates &result) {
				session->api().applyUpdates(result);
				self(self, i + 1);
			}).fail([=](const MTP::Error &) {
				self(self, i + 1);
			}).send();
			return;
		}
	};
	unpin(unpin, 0);
}

} // namespace

void InsertBatchActions(
		Ui::PopupMenu *menu,
		HistoryItem *item,
		Window::SessionController *controller,
		MessageIdsList selected,
		SelectionTarget selection) {
	if (!menu || !controller) {
		return;
	}
	auto position = EndPosition(menu);
	if (const auto pinned = PinnedAmong(&controller->session(), selected)
		; pinned.size() > 1) {
		const auto session = &controller->session();
		Insert(menu, position++, ActionId::BatchUnpin,
			tr::lng_serein_menu_unpin_selected(tr::now),
			crl::guard(controller, [=] { UnpinSequentially(session, pinned); }));
	}
	if (selection && selected.size() > 1) {
		Insert(menu, position++, ActionId::SelectRange,
			tr::lng_serein_menu_select_range(tr::now),
			crl::guard(controller, [=] { selection.selectRange(); }));
	}
	if (selected.size() > 1 && selected.size() <= kMaximumMessages) {
		const auto sourceId = selected.front();
		Insert(menu, position++, ActionId::Batch,
			tr::lng_serein_menu_batch(tr::now),
			crl::guard(controller, [=] {
				controller->show(Box(BatchBox, controller, selected, sourceId));
			}));
	}
	if (item && selection && item->canBeSelected()) {
		const auto itemId = item->fullId();
		Insert(menu, position, ActionId::SelectSender,
			tr::lng_serein_menu_select_sender(tr::now),
			crl::guard(controller, [=] {
				if (const auto current = controller->session().data().message(itemId)) {
					selection.selectSender(current);
				}
			}));
	}
}

} // namespace Serein::Menu
