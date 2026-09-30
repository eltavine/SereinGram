#pragma once

#include "serein/schema/gen/config/main_menu.h"
#include "base/basic_types.h"

#include <rpl/producer.h>
#include <QtCore/QByteArray>
#include <gsl/pointers>

#include <optional>
#include <array>

namespace Ui {
class GenericBox;
class SettingsButton;
class VerticalLayout;
} // namespace Ui

namespace Settings {
struct IconDescriptor;
} // namespace Settings

namespace Window {
class SessionController;
} // namespace Window

namespace Serein::Interface {

inline constexpr auto kMainMenuIds = std::array{
	"profile", "bots", "newGroup", "newChannel", "contacts", "calls",
	"savedMessages", "settings", "nightMode",
};

[[nodiscard]] QString MainMenuActionTitle(const QString &id);
[[nodiscard]] MainMenuConfig MainMenuDefaults();
[[nodiscard]] bool ValidMainMenuBytes(const QByteArray &value);
[[nodiscard]] std::optional<MainMenuConfig> MainMenu();
void SetMainMenu(const MainMenuConfig &value);
[[nodiscard]] QString MainMenuTitle();
[[nodiscard]] bool MainMenuSeasonal();
[[nodiscard]] bool MainMenuCustomOrder();
[[nodiscard]] gsl::not_null<Ui::VerticalLayout*> AddMainMenuGroup(
	gsl::not_null<Ui::VerticalLayout*> menu,
	const QString &id);
void MainMenuBox(gsl::not_null<Ui::GenericBox*> box);

} // namespace Serein::Interface

namespace Serein::Hooks {

using MainMenuAction = Fn<gsl::not_null<Ui::SettingsButton*>(
	rpl::producer<QString>,
	Settings::IconDescriptor &&)>;

void FillMainMenu(
	gsl::not_null<Window::SessionController*> controller,
	const MainMenuAction &addAction);

} // namespace Serein::Hooks
