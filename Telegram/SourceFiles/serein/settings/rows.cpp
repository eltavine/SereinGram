#include "serein/settings/rows.h"

#include "serein/settings/restart.h"
#include "ui/widgets/buttons.h"
#include "window/window_session_controller.h"
#include "styles/style_settings.h"

namespace Serein {
namespace {

[[nodiscard]] Fn<Options&()> StoreFor(
		::Settings::Builder::SectionBuilder &builder,
		const Option<bool> &option) {
	if (option.scope == Scope::Account) {
		const auto session = builder.session();
		return [=]() -> Options & { return ForAccount(session); };
	}
	return []() -> Options & { return ForDevice(); };
}

} // namespace

void AddToggle(
		::Settings::Builder::SectionBuilder &builder,
		const ToggleRow &row) {
	Expects(row.option != nullptr);

	const auto option = *row.option;
	const auto store = StoreFor(builder, option);
	const auto controller = builder.controller();
	const auto button = builder.addButton({
		.id = row.id,
		.title = row.title(),
		.st = &st::settingsButtonNoIcon,
		.toggled = store().Value(option),
		.keywords = row.keywords,
	});
	if (!button) {
		return;
	}
	button->toggledChanges(
	) | rpl::on_next([=](bool value) {
		Expects(store().Set(option, value));
		if (controller
			&& (option.flags & static_cast<unsigned>(Flag::RequiresRestart))) {
			ShowRestartPrompt(controller);
		}
	}, button->lifetime());
	if (const auto source = row.disabledBy) {
		StoreFor(builder, *source)().Value(
			*source
		) | rpl::on_next([=](bool disabled) {
			button->setDisabled(disabled);
			button->setEnabled(!disabled);
		}, button->lifetime());
	}
}

void AddToggles(
		::Settings::Builder::SectionBuilder &builder,
		std::span<const ToggleRow> rows) {
	for (const auto &row : rows) {
		AddToggle(builder, row);
	}
}

void AddSection(
		::Settings::Builder::SectionBuilder &builder,
		const SectionRow &row) {
	builder.addSubsectionTitle({
		.id = row.id,
		.title = row.title(),
		.keywords = row.keywords,
	});
}

void AddNote(
		::Settings::Builder::SectionBuilder &builder,
		tr::phrase<> text) {
	builder.addDividerText(text());
}

} // namespace Serein
