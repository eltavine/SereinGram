#pragma once

#include "base/basic_types.h"
#include "lang/lang_keys.h"
#include "serein/core/options.h"
#include "settings/settings_builder.h"

#include <optional>
#include <span>
#include <type_traits>
#include <vector>

namespace Ui {
class SettingsButton;
} // namespace Ui

namespace Serein {

struct RowVisual {
	const style::icon *icon = nullptr;
	const style::color *tile = nullptr;
	std::optional<tr::phrase<>> about;
};

struct ToggleRow {
	const Option<bool> *option = nullptr;
	tr::phrase<> title;
	QString id;
	QStringList keywords;
	const Option<bool> *disabledBy = nullptr;
	const style::icon *icon = nullptr;
	const style::color *tile = nullptr;
	std::optional<tr::phrase<>> about;
};

struct SectionRow {
	QString id;
	tr::phrase<> title;
	QStringList keywords;
};

struct NumberRow {
	const Option<int> *option = nullptr;
	tr::phrase<> title;
	QString id;
	QStringList keywords;
	int minimum = 0;
	int maximum = 0;
	tr::phrase<> zeroLabel;
	Fn<QString(int)> format;
	std::optional<tr::phrase<>> hint;
	const style::icon *icon = nullptr;
	const style::color *tile = nullptr;
	std::optional<tr::phrase<>> about;
};

struct ChoiceRow {
	const Option<int> *option = nullptr;
	tr::phrase<> title;
	QString id;
	QStringList keywords;
	std::vector<int> values;
	std::vector<tr::phrase<>> labels;
	QString suffix;
	const style::icon *icon = nullptr;
	const style::color *tile = nullptr;
	std::optional<tr::phrase<>> about;
};

struct TextRow {
	const Option<QString> *option = nullptr;
	tr::phrase<> title;
	QString id;
	QStringList keywords;
	tr::phrase<> placeholder;
	const Option<bool> *hiddenBy = nullptr;
	const style::icon *icon = nullptr;
	const style::color *tile = nullptr;
	std::optional<tr::phrase<>> about;
};

struct RowArgs {
	QString id;
	rpl::producer<QString> title;
	rpl::producer<QString> label;
	rpl::producer<bool> toggled;
	Fn<void()> onClick;
	QStringList keywords;
	rpl::producer<bool> shown;
	RowVisual visual;
	const style::SettingsButton *st = nullptr;
};

struct PageButton {
	rpl::producer<QString> title;
	::Settings::Type section;
	const style::icon *icon = nullptr;
	const style::color *tile = nullptr;
	QStringList keywords;
	std::optional<tr::phrase<>> about;
};

class CustomRow final {
public:
	template <typename Callback>
		requires (!std::is_same_v<std::decay_t<Callback>, CustomRow>)
	CustomRow(Callback &&callback)
	: _build(std::forward<Callback>(callback)) {
	}

	void operator()() const {
		_build();
	}

private:
	Fn<void()> _build;

};

// The row is null while the settings search builds its index.
Ui::SettingsButton *AddRow(
	::Settings::Builder::SectionBuilder &builder,
	RowArgs &&args);
void AddPageButton(
	::Settings::Builder::SectionBuilder &builder,
	PageButton &&button);
void AddToggle(
	::Settings::Builder::SectionBuilder &builder,
	const ToggleRow &row);
void AddToggles(
	::Settings::Builder::SectionBuilder &builder,
	std::span<const ToggleRow> rows);
void AddSection(
	::Settings::Builder::SectionBuilder &builder,
	const SectionRow &row);
void AddNumber(
	::Settings::Builder::SectionBuilder &builder,
	const NumberRow &row);
void AddChoice(
	::Settings::Builder::SectionBuilder &builder,
	const ChoiceRow &row);
void AddText(
	::Settings::Builder::SectionBuilder &builder,
	const TextRow &row);
void AddNote(
	::Settings::Builder::SectionBuilder &builder,
	tr::phrase<> text);
void EndSection(::Settings::Builder::SectionBuilder &builder);
void EndSection(
	::Settings::Builder::SectionBuilder &builder,
	tr::phrase<> note);

} // namespace Serein
