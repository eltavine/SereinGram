#pragma once

#include "history/view/history_view_bottom_info.h"
#include "ui/text/text_entity.h"

class HistoryItem;

namespace HistoryView {
class Element;
} // namespace HistoryView

namespace Nagram::Messages {

[[nodiscard]] QString FormatTime(QTime time);
[[nodiscard]] QString FormatSavedFrom(QDateTime dateTime);
void ApplyForwardedDate(
	HistoryView::BottomInfo::Data &data,
	not_null<HistoryItem*> item);
[[nodiscard]] TextWithEntities ServiceText(
	not_null<HistoryView::Element*> view,
	const TextWithEntities &text);
[[nodiscard]] QString WithMessageId(
	QString text,
	not_null<HistoryItem*> item);

} // namespace Nagram::Messages
