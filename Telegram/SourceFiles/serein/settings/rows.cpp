#include "serein/settings/rows.h"

#include "serein/settings/restart.h"
#include "ui/widgets/buttons.h"
#include "window/window_session_controller.h"
#include "styles/style_settings.h"

namespace Serein {

void AddToggle(
		::Settings::Builder::SectionBuilder &builder,
		const ToggleRow &row) {
	Expects(row.option != nullptr);

	const auto option = *row.option;
	const auto session = (option.scope == Scope::Account)
		? builder.session().get()
		: nullptr;
	const auto options = [=]() -> Options & {
		return session ? ForAccount(session) : ForDevice();
	};
	const auto controller = builder.controller();
	const auto button = builder.addButton({
		.id = row.id,
		.title = row.title(),
		.st = &st::settingsButtonNoIcon,
		.toggled = options().Value(option),
		.keywords = row.keywords,
	});
	if (button) {
		button->toggledChanges(
		) | rpl::on_next([=](bool value) {
			Expects(options().Set(option, value));
			if (controller
				&& (option.flags & static_cast<unsigned>(Flag::RequiresRestart))) {
				ShowRestartPrompt(controller);
			}
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

} // namespace Serein
