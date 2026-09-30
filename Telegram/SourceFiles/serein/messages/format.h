#pragma once

#include "history/view/history_view_bottom_info.h"
#include "ui/text/text_entity.h"

class HistoryItem;

namespace HistoryView {
class Element;
} // namespace HistoryView

namespace Serein::Messages {

[[nodiscard]] QString FormatTime(QTime time);
[[nodiscard]] QString FormatSavedFrom(QDateTime dateTime);
[[nodiscard]] QString FormatEditedDate(QDateTime sent, QDateTime edited);
[[nodiscard]] QString EditedMark();
[[nodiscard]] QString FormatCounter(int count);
void ApplyInfoOptions(HistoryView::BottomInfo::Data &data);
void ApplyForwardedDate(
	HistoryView::BottomInfo::Data &data,
	not_null<HistoryItem*> item);
[[nodiscard]] TextWithEntities ServiceText(
	not_null<HistoryView::Element*> view,
	const TextWithEntities &text);
[[nodiscard]] QString WithMessageId(
	QString text,
	not_null<HistoryItem*> item);

} // namespace Serein::Messages
