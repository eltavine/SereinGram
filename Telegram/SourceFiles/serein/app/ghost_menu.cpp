#include "serein/hooks/ghost.h"

#include "serein/features/ghost/model/policy.h"
#include "serein/privacy/options.h"
#include "ui/widgets/buttons.h"

namespace Serein::Hooks {

void BindGhostToggle(
		gsl::not_null<Ui::SettingsButton*> button,
		gsl::not_null<Main::Session*> session) {
	button->toggleOn(rpl::combine(
		ForAccount(session).Value(Serein::Ghost::kGhostMode),
		ForDevice().Value(Serein::Ghost::kGhostAllAccounts)
	) | rpl::map([](bool account, bool all) {
		return account || all;
	}));
	button->toggledChanges(
	) | rpl::filter([=](bool enabled) {
		return enabled
			!= Serein::Ghost::Enabled(ForAccount(session), ForDevice());
	}) | rpl::on_next([=](bool enabled) {
		Expects(Serein::Ghost::SetEnabled(
			ForAccount(session),
			ForDevice(),
			enabled));
	}, button->lifetime());
}

void BindStreamerToggle(gsl::not_null<Ui::SettingsButton*> button) {
	button->toggleOn(ForDevice().Value(Privacy::kDemoMode));
	button->toggledChanges(
	) | rpl::filter([=](bool enabled) {
		return enabled != ForDevice().Get(Privacy::kDemoMode);
	}) | rpl::on_next([=](bool enabled) {
		Expects(ForDevice().Set(Privacy::kDemoMode, enabled));
	}, button->lifetime());
}

} // namespace Serein::Hooks
