#include "serein/hooks/ghost.h"

#include "serein/features/ghost/model/policy.h"
#include "ui/widgets/buttons.h"

namespace Serein::Hooks {

void BindGhostToggle(
		gsl::not_null<Ui::SettingsButton*> button,
		gsl::not_null<Main::Session*> session) {
	button->toggleOn(ForAccount(session).Value(Ghost::kGhostMode));
	button->toggledChanges(
	) | rpl::filter([=](bool enabled) {
		return enabled != ForAccount(session).Get(Ghost::kGhostMode);
	}) | rpl::on_next([=](bool enabled) {
		Expects(ForAccount(session).Set(Ghost::kGhostMode, enabled));
	}, button->lifetime());
}

} // namespace Serein::Hooks
