#include "serein/hooks/menu/actions.h"

#include "serein/menu/model.h"
#include "data/data_types.h"
#include "serein/menu/buttons.h"
#include "serein/menu/details.h"
#include "serein/menu/rating.h"
#include "serein/menu/repeat.h"
#include "serein/menu/batch.h"
#include "serein/menu/media.h"
#include "serein/menu/reading.h"
#include "serein/menu/ghost_read.h"
#include "serein/menu/history.h"
#include "serein/filters/menu.h"
#include "serein/snapshot/snapshot.h"

#include "lang/lang_keys.h"
#include "ui/widgets/popup_menu.h"

#include <QtGui/QAction>
#include <QtGui/QGuiApplication>

#include <optional>

namespace Serein::Menu {
namespace {

constexpr auto kActionIdProperty = "sereinMenuActionId";

using UpstreamTexts = std::vector<std::pair<QString, ActionId>>;

[[nodiscard]] UpstreamTexts CollectUpstreamTexts() {
	using Phrase = std::pair<const tr::phrase<>*, ActionId>;
	const auto phrases = {
		Phrase{ &tr::lng_context_reply_msg, ActionId::Reply },
		Phrase{ &tr::lng_context_quote_and_reply, ActionId::Reply },
		Phrase{ &tr::lng_context_reply_to_task, ActionId::Reply },
		Phrase{ &tr::lng_context_edit_msg, ActionId::Edit },
		Phrase{ &tr::lng_context_copy_text, ActionId::Copy },
		Phrase{ &tr::lng_context_copy_selected, ActionId::Copy },
		Phrase{ &tr::lng_context_copy_selected_items, ActionId::Copy },
		Phrase{ &tr::lng_context_copy_post_link, ActionId::CopyLink },
		Phrase{ &tr::lng_context_copy_message_link, ActionId::CopyLink },
		Phrase{ &tr::lng_context_forward_msg, ActionId::Forward },
		Phrase{ &tr::lng_context_forward_selected, ActionId::Forward },
		Phrase{ &tr::lng_context_translate, ActionId::Translate },
		Phrase{ &tr::lng_context_translate_selected, ActionId::Translate },
		Phrase{ &tr::lng_context_pin_msg, ActionId::Pin },
		Phrase{ &tr::lng_context_unpin_msg, ActionId::Pin },
		Phrase{ &tr::lng_context_unpin_selected, ActionId::Pin },
		Phrase{ &tr::lng_context_select_msg, ActionId::Select },
		Phrase{ &tr::lng_context_select_msg_bulk, ActionId::Select },
		Phrase{ &tr::lng_context_clear_selection, ActionId::Select },
		Phrase{ &tr::lng_stats_title, ActionId::Statistics },
		Phrase{ &tr::lng_context_report_msg, ActionId::Report },
		Phrase{ &tr::lng_profile_block_user, ActionId::BlockSender },
		Phrase{ &tr::lng_context_delete_selected, ActionId::Delete },
		Phrase{ &tr::lng_context_attached_stickers, ActionId::StickerPack },
		Phrase{ &tr::lng_context_pack_info, ActionId::StickerPack },
		Phrase{ &tr::lng_context_pack_add, ActionId::StickerPack },
	};
	auto result = UpstreamTexts();
	result.reserve(phrases.size());
	for (const auto &[phrase, id] : phrases) {
		result.emplace_back((*phrase)(tr::now), id);
	}
	return result;
}

[[nodiscard]] std::optional<ActionId> ActionOf(
		not_null<QAction*> action,
		const UpstreamTexts &texts) {
	const auto value = action->property(kActionIdProperty);
	if (value.isValid()) {
		return ActionId(value.toInt());
	} else if (action->isSeparator()) {
		return std::nullopt;
	}
	const auto text = action->text();
	for (const auto &[upstream, id] : texts) {
		if (upstream == text) {
			return id;
		}
	}
	return std::nullopt;
}

} // namespace

int DeleteActionIndex(Ui::PopupMenu *menu) {
	Expects(menu != nullptr);
	const auto texts = CollectUpstreamTexts();
	for (auto index = 0; index != int(menu->actions().size()); ++index) {
		if (ActionOf(menu->actions()[index], texts) == ActionId::Delete) {
			return index;
		}
	}
	return int(menu->actions().size());
}

void Tag(QAction *action, ActionId id) {
	Expects(action != nullptr);
	action->setProperty(kActionIdProperty, static_cast<int>(id));
}

void Apply(
		Ui::PopupMenu *menu,
		HistoryItem *item,
		Window::SessionController *controller,
		MessageIdsList selected,
		SelectionTarget selection) {
	Expects(menu != nullptr);
	if (controller) {
		if (item) {
			InsertQuickRatingActions(menu, item, controller);
			InsertRepeatActions(menu, item, controller);
		}
		if (item || !selected.empty()) {
			InsertBatchActions(menu, item, controller,
				selected, selection);
			Snapshot::InsertAction(menu, controller, item, selected);
		}
		if (item) {
			InsertMediaInfoAction(menu, item, controller);
			InsertReadingAction(menu, item, controller);
			Filters::InsertAuthorAction(menu, item, controller);
			InsertEditHistoryAction(menu, item, controller);
			InsertDeletedMessagesAction(menu, item, controller);
			InsertReadUntilHereAction(menu, item, controller);
			InsertHistoryExclusionAction(menu, item, controller);
			InsertButtonDataAction(menu, item, controller);
			InsertDetailsAction(menu, item, controller);
		}
	}
	const auto config = ForDevice().Get(kMenuConfig);
	const auto optionHeld = (QGuiApplication::keyboardModifiers()
		& Qt::AltModifier) != 0;
	auto removedUpstreamAction = false;
	const auto texts = CollectUpstreamTexts();
	for (auto index = int(menu->actions().size()); index != 0;) {
		--index;
		const auto id = ActionOf(menu->actions()[index], texts);
		if (id && !Visible(ReadVisibility(config, *id), optionHeld)) {
			removedUpstreamAction |= (*id < ActionId::Repeat);
			menu->removeAction(index);
		}
	}
	if (!removedUpstreamAction) {
		return;
	}
	auto previousSeparator = true;
	for (auto index = 0; index != int(menu->actions().size());) {
		const auto separator = menu->actions()[index]->isSeparator();
		if (separator && previousSeparator) {
			menu->removeAction(index);
		} else {
			previousSeparator = separator;
			++index;
		}
	}
	if (!menu->actions().empty() && menu->actions().back()->isSeparator()) {
		menu->removeAction(int(menu->actions().size()) - 1);
	}
}

} // namespace Serein::Menu
