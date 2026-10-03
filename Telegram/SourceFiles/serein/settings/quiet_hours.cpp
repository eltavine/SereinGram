#include "serein/settings/quiet_hours.h"

#include "serein/core/options.h"
#include "serein/features/quiet_hours/model/schedule.h"
#include "serein/schema/gen/settings/interface.h"
#include "base/basic_types.h"
#include "lang/lang_keys.h"
#include "settings/settings_builder.h"
#include "ui/layers/generic_box.h"
#include "ui/ui_utility.h"
#include "ui/widgets/checkbox.h"
#include "ui/widgets/labels.h"
#include "ui/widgets/time_input.h"
#include "ui/wrap/padding_wrap.h"
#include "window/window_session_controller.h"

#include <QtCore/QTime>

#include "styles/style_layers.h"
#include "styles/style_passcode_box.h"
#include "styles/style_settings.h"

#include <algorithm>
#include <array>

namespace Serein::Interface {
namespace {

using Notifications::QuietHours;

constexpr auto kWeekdays = 7;
constexpr auto kWeekdayColumns = 4;

[[nodiscard]] QuietHours Read(const QByteArray &raw) {
	return Notifications::ReadQuietHours(raw).value_or(QuietHours());
}

[[nodiscard]] QString FormatMinute(int minute) {
	return QTime(minute / 60, minute % 60).toString(u"H:mm"_q);
}

[[nodiscard]] std::optional<int> ParseMinute(const QString &text) {
	const auto time = QTime::fromString(text, u"H:mm"_q);
	return time.isValid()
		? std::make_optional(time.hour() * 60 + time.minute())
		: std::nullopt;
}

[[nodiscard]] QString Describe(const QuietHours &config) {
	return config.enabled
		? (FormatMinute(config.startMinute)
			+ QChar(0x2013)
			+ FormatMinute(config.endMinute))
		: tr::lng_serein_config_off(tr::now);
}

[[nodiscard]] QString WeekdayName(int weekday) {
	static const auto names = std::array{
		tr::lng_weekday1,
		tr::lng_weekday2,
		tr::lng_weekday3,
		tr::lng_weekday4,
		tr::lng_weekday5,
		tr::lng_weekday6,
		tr::lng_weekday7,
	};
	return names[weekday - 1](tr::now);
}

not_null<Ui::TimeInput*> AddTime(
		not_null<Ui::GenericBox*> box,
		rpl::producer<QString> label,
		int minute) {
	const auto row = box->addRow(object_ptr<Ui::FixedHeightWidget>(box));
	const auto title = Ui::CreateChild<Ui::FlatLabel>(
		row,
		std::move(label),
		st::boxLabel);
	const auto input = Ui::CreateChild<Ui::TimeInput>(
		row,
		FormatMinute(minute),
		st::autolockTimeField,
		st::autolockDateField,
		st::scheduleTimeSeparator,
		st::scheduleTimeSeparatorPadding);
	input->resizeToWidth(st::autolockTimeWidth);
	row->widthValue() | rpl::on_next([=](int width) {
		title->resizeToWidth(width - input->width());
		row->resize(width, std::max(input->height(), title->height()));
		title->moveToLeft(0, (row->height() - title->height()) / 2);
		input->moveToRight(0, 0, width);
	}, row->lifetime());
	return input;
}

std::vector<not_null<Ui::Checkbox*>> AddWeekdays(
		not_null<Ui::GenericBox*> box,
		const QuietHours &config) {
	const auto row = box->addRow(object_ptr<Ui::FixedHeightWidget>(box));
	auto result = std::vector<not_null<Ui::Checkbox*>>();
	for (auto weekday = 1; weekday <= kWeekdays; ++weekday) {
		const auto selected = config.weekdays.empty()
			|| std::ranges::find(config.weekdays, weekday)
				!= config.weekdays.end();
		result.push_back(Ui::CreateChild<Ui::Checkbox>(
			row,
			WeekdayName(weekday),
			selected));
	}
	row->widthValue() | rpl::on_next([=](int width) {
		const auto column = width / kWeekdayColumns;
		const auto height = result.front()->height();
		for (auto i = 0; i != kWeekdays; ++i) {
			result[i]->resizeToNaturalWidth(column);
			result[i]->moveToLeft(
				(i % kWeekdayColumns) * column,
				(i / kWeekdayColumns) * height);
		}
		const auto rows = (kWeekdays + kWeekdayColumns - 1) / kWeekdayColumns;
		row->resize(width, rows * height);
	}, row->lifetime());
	return result;
}

void QuietHoursBox(not_null<Ui::GenericBox*> box) {
	box->setTitle(tr::lng_serein_quiet_hours());
	const auto config = Read(ForDevice().Get(kQuietHours));
	const auto enabled = box->addRow(object_ptr<Ui::Checkbox>(
		box,
		tr::lng_serein_quiet_hours_enable(tr::now),
		config.enabled));
	const auto start = AddTime(
		box,
		tr::lng_serein_quiet_hours_from(),
		config.startMinute);
	const auto end = AddTime(
		box,
		tr::lng_serein_quiet_hours_till(),
		config.endMinute);
	box->addRow(object_ptr<Ui::FlatLabel>(
		box,
		tr::lng_serein_quiet_hours_days(),
		st::boxLabel));
	const auto weekdays = AddWeekdays(box, config);
	box->addRow(object_ptr<Ui::FlatLabel>(
		box,
		tr::lng_serein_quiet_hours_allow(),
		st::boxLabel));
	const auto allow = [&](tr::phrase<> label, bool checked) {
		return box->addRow(object_ptr<Ui::Checkbox>(
			box,
			label(tr::now),
			checked));
	};
	const auto contacts = allow(
		tr::lng_serein_quiet_hours_allow_contacts,
		config.allowContacts);
	const auto pinned = allow(
		tr::lng_serein_quiet_hours_allow_pinned,
		config.allowPinned);
	const auto mentions = allow(
		tr::lng_serein_quiet_hours_allow_mentions,
		config.allowMentions);
	const auto keywords = allow(
		tr::lng_serein_quiet_hours_allow_keywords,
		config.allowKeywords);
	box->addRow(object_ptr<Ui::FlatLabel>(
		box,
		tr::lng_serein_quiet_hours_about(),
		st::boxDividerLabel));

	box->addButton(tr::lng_settings_save(), [=] {
		const auto from = ParseMinute(start->valueCurrent());
		const auto till = ParseMinute(end->valueCurrent());
		if (!from || !till) {
			(from ? end : start)->showError();
			return;
		}
		auto result = QuietHours{
			.enabled = enabled->checked(),
			.startMinute = *from,
			.endMinute = *till,
			.allowContacts = contacts->checked(),
			.allowPinned = pinned->checked(),
			.allowMentions = mentions->checked(),
			.allowKeywords = keywords->checked(),
		};
		for (auto i = 0; i != kWeekdays; ++i) {
			if (weekdays[i]->checked()) {
				result.weekdays.push_back(i + 1);
			}
		}
		if (result.weekdays.empty()) {
			box->showToast(tr::lng_serein_quiet_hours_no_days(tr::now));
			return;
		} else if (result.weekdays.size() == kWeekdays) {
			result.weekdays.clear();
		}
		Expects(ForDevice().Set(kQuietHours, (result == QuietHours())
			? QByteArray()
			: Notifications::SerializeQuietHours(result)));
		box->closeBox();
	});
	box->addButton(tr::lng_cancel(), [=] { box->closeBox(); });
}

} // namespace

void AddQuietHours(::Settings::Builder::SectionBuilder &builder) {
	const auto controller = builder.controller();
	builder.addButton({
		.id = u"serein/interface/quiet-hours"_q,
		.title = tr::lng_serein_quiet_hours(),
		.st = &st::settingsButtonNoIcon,
		.label = ForDevice().Value(kQuietHours) | rpl::map([](QByteArray raw) {
			return Describe(Read(raw));
		}),
		.onClick = [=] { controller->show(Box(QuietHoursBox)); },
		.keywords = { u"quiet"_q, u"do not disturb"_q, u"schedule"_q },
	});
}

} // namespace Serein::Interface
