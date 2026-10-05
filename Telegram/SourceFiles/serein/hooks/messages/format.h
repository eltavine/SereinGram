#pragma once

#include <QtCore/QDateTime>
#include <QtCore/QString>
#include <QtCore/QTime>

#include <gsl/pointers>

class HistoryItem;
class Painter;
struct TextWithEntities;

namespace HistoryView {
class Element;
} // namespace HistoryView

namespace style {
struct TextPalette;
} // namespace style

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

class MarksPalette final {
public:
	MarksPalette(Painter &p, bool active);
	MarksPalette(const MarksPalette &) = delete;
	MarksPalette &operator=(const MarksPalette &) = delete;
	~MarksPalette();

private:
	Painter &_p;
	const style::TextPalette *_previous = nullptr;

};

} // namespace Serein::Messages
