#pragma once

#include <QtCore/QDateTime>
#include <QtCore/QString>
#include <QtCore/QTime>

#include <gsl/pointers>

class HistoryItem;
struct TextWithEntities;

namespace HistoryView {
class Element;
} // namespace HistoryView

namespace Serein::Messages {

[[nodiscard]] QString FormatTime(QTime time);
[[nodiscard]] QString FormatSavedFrom(QDateTime dateTime);
[[nodiscard]] QString FormatEditedDate(QDateTime sent, QDateTime edited);
[[nodiscard]] QString EditedMark();
[[nodiscard]] QString FormatCounter(int count);
template <typename Data>
void ApplyInfoOptions(Data &data, gsl::not_null<HistoryItem*> item);
template <typename Data>
void ApplyForwardedDate(Data &data, gsl::not_null<HistoryItem*> item);
[[nodiscard]] TextWithEntities ServiceText(
	gsl::not_null<HistoryView::Element*> view,
	const TextWithEntities &text);
[[nodiscard]] QString WithMessageId(
	QString text,
	gsl::not_null<HistoryItem*> item);

} // namespace Serein::Messages
