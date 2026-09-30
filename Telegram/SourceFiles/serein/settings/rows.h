#pragma once

#include "base/basic_types.h"
#include "lang/lang_keys.h"
#include "serein/core/options.h"
#include "settings/settings_builder.h"

#include <span>
#include <type_traits>

namespace Serein {

struct ToggleRow {
	const Option<bool> *option = nullptr;
	tr::phrase<> title;
	QString id;
	QStringList keywords;
	const Option<bool> *disabledBy = nullptr;
};

struct SectionRow {
	QString id;
	tr::phrase<> title;
	QStringList keywords;
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

void AddToggle(
	::Settings::Builder::SectionBuilder &builder,
	const ToggleRow &row);
void AddToggles(
	::Settings::Builder::SectionBuilder &builder,
	std::span<const ToggleRow> rows);
void AddSection(
	::Settings::Builder::SectionBuilder &builder,
	const SectionRow &row);
void AddNote(
	::Settings::Builder::SectionBuilder &builder,
	tr::phrase<> text);

} // namespace Serein
