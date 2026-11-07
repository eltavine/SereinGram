#include "serein/hooks/messages/format.h"

#include "serein/core/options.h"
#include "serein/features/history/deleted_marks.h"
#include "serein/messages/options.h"
#include "serein/hooks/messages/time_format.h"
#include "serein/schema/gen/settings/history.h"
#include "history/history.h"
#include "history/history_item.h"
#include "history/history_item_components.h"
#include "history/view/history_view_bottom_info.h"
#include "history/view/history_view_element.h"
#include "main/main_session.h"
#include "ui/painter.h"
#include "ui/text/format_values.h"
#include "ui/text/text_utilities.h"
#include "lang/lang_keys.h"
#include "base/unixtime.h"
#include "styles/style_serein.h"

#include <QtCore/QLocale>

#include <cstdlib>

namespace Serein::Messages {
namespace {

[[nodiscard]] QString DeletedMark() {
	const auto custom = ForDevice().Get(kDeletedMark);
	return custom.isEmpty() ? tr::lng_serein_deleted_mark(tr::now) : custom;
}

[[nodiscard]] TextWithEntities HighlightedMark(
		const style::IconEmoji &icon,
		const QString &text) {
	return Ui::Text::Colorized(
		Ui::Text::IconEmoji(&icon).append(text)
	).append(QChar(' '));
}

[[nodiscard]] bool RecordsEdits(not_null<HistoryItem*> item) {
	return ForAccount(&item->history()->session()).Get(
		HistorySettings::kHistorySaveEdits);
}

} // namespace

QString FormatTime(QTime time) {
	return FormatTime(time, ForDevice().Get(kSecondsInMessages));
}

QString FormatSavedFrom(QDateTime dateTime) {
	if (!ForDevice().Get(kSecondsInMessages)) {
		return Ui::FormatDateTimeSavedFrom(dateTime);
	}
	const auto current = QDate::currentDate();
	const auto date = dateTime.date();
	const auto time = FormatTime(dateTime.time());
	if (date == current) {
		return tr::lng_mediaview_today(tr::now, lt_time, time);
	} else if (date == current.addDays(-1)) {
		return tr::lng_mediaview_yesterday(tr::now, lt_time, time);
	}
	constexpr auto kSecondsInYear = 365 * 24 * 60 * 60;
	const auto diff = std::abs(
		base::unixtime::now() - base::unixtime::serialize(dateTime));
	const auto dateText = (diff < kSecondsInYear)
		? tr::lng_month_day(
			tr::now,
			lt_month,
			Lang::MonthSmall(date.month())(tr::now),
			lt_day,
			QString::number(date.day()))
		: langDayOfMonthFull(date);
	return tr::lng_mediaview_date_time(
		tr::now, lt_date, dateText, lt_time, time);
}

QString FormatEditedDate(QDateTime sent, QDateTime edited) {
	const auto today = QDateTime::currentDateTime().date();
	const auto time = FormatTime(edited.time());
	const auto mark = ForDevice().Get(kEditedMark);
	if (sent.date() == today && edited.date() == today) {
		return mark.isEmpty()
			? tr::lng_edited_at(tr::now, lt_time, time)
			: mark + ' ' + time;
	}
	const auto date = langDayOfMonthShort(edited.date());
	return mark.isEmpty()
		? tr::lng_edited_on(tr::now, lt_date, date, lt_time, time)
		: mark + ' ' + date + ' ' + time;
}

QString EditedMark() {
	const auto mark = ForDevice().Get(kEditedMark);
	return mark.isEmpty() ? tr::lng_edited(tr::now) : mark;
}

QString FormatCounter(int count) {
	return ForDevice().Get(kExactMessageCounters)
		? Lang::FormatCountDecimal(count)
		: Lang::FormatCountToShort(count).string;
}

template <typename Data>
void ApplyInfoOptions(Data &data, not_null<HistoryItem*> item) {
	using Flag = HistoryView::BottomInfo::Data::Flag;
	auto &options = ForDevice();
	if (options.Get(kHideMessageViews)) {
		data.views.reset();
	}
	if (options.Get(kHideChannelSignature)) {
		data.author.clear();
	}
	const auto hideEdited = options.Get(kHideEditedBadge);
	if (hideEdited) {
		data.flags &= ~(Flag::Edited | Flag::EditedPrimary);
	}
	const auto marks = HistoryFeature::MarksFor(item);
	const auto highlight = options.Get(kHighlightHistoryMarks);
	if (marks.deleted && highlight) {
		data.sereinMarks.append(
			HighlightedMark(st::sereinHistoryDeletedMark, DeletedMark()));
	} else if (marks.deleted) {
		const auto mark = DeletedMark();
		data.author = data.author.isEmpty()
			? mark
			: (mark + u" \u00B7 "_q + data.author);
	}
	const auto edited = (data.flags & (Flag::Edited | Flag::EditedPrimary))
		&& RecordsEdits(item);
	if (highlight && !hideEdited && (marks.edited || edited)) {
		if (data.flags & Flag::EditedPrimary) {
			data.date = data.editedDate;
		}
		data.flags &= ~(Flag::Edited | Flag::EditedPrimary);
		data.sereinMarks.append(
			HighlightedMark(st::sereinHistoryEditedMark, EditedMark()));
	}
}

template void ApplyInfoOptions(
	HistoryView::BottomInfo::Data &data,
	not_null<HistoryItem*> item);

template <typename Data>
void ApplyForwardedDate(Data &data, not_null<HistoryItem*> item) {
	if (!ForDevice().Get(kShowForwardedMessageDate)
		|| item->externalReply()) {
		return;
	}
	const auto forwarded = item->Get<HistoryMessageForwarded>();
	if (!forwarded || !forwarded->originalDate) {
		return;
	}
	data.date = base::unixtime::parse(forwarded->originalDate);
	data.flags |= HistoryView::BottomInfo::Data::Flag::ForwardedDate;
}

template void ApplyForwardedDate(
	HistoryView::BottomInfo::Data &data,
	not_null<HistoryItem*> item);

TextWithEntities ServiceText(
		not_null<HistoryView::Element*> view,
		const TextWithEntities &text) {
	if (!ForDevice().Get(kShowServiceTime)
		|| text.empty()
		|| view->data()->date() <= 0) {
		return text;
	}
	auto result = text;
	result.text += u" · "_q + FormatTime(view->dateTime().time());
	return result;
}

MarksPalette::MarksPalette(Painter &p, bool active)
: _p(p)
, _previous(active ? &p.textPalette() : nullptr) {
	if (active) {
		p.setTextPalette(st::sereinHistoryMarkPalette);
	}
}

MarksPalette::~MarksPalette() {
	if (_previous) {
		_p.setTextPalette(*_previous);
	}
}

QString WithMessageId(QString text, not_null<HistoryItem*> item) {
	if (ForDevice().Get(kShowMessageId)
		&& IsServerMsgId(item->id)
		&& !item->isSending()) {
		text += '\n' + tr::lng_serein_message_id(
			tr::now, lt_id, QString::number(item->id.bare));
	}
	return text;
}

} // namespace Serein::Messages
