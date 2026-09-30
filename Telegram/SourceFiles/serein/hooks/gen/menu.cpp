// Generated from proto/serein/settings/v1/menu.proto by tools/serein/codegen; do not edit.
#include "serein/hooks/gen/menu.h"

#include "serein/core/options.h"
#include "serein/schema/gen/settings/menu.h"

namespace Serein::Hooks::Menu {

QByteArray MenuConfig() {
	return ForDevice().Get(Serein::Menu::kMenuConfig);
}

rpl::producer<QByteArray> MenuConfigValue() {
	return ForDevice().Value(Serein::Menu::kMenuConfig);
}

bool ConfirmRepeat() {
	return ForDevice().Get(Serein::Menu::kConfirmRepeat);
}

rpl::producer<bool> ConfirmRepeatValue() {
	return ForDevice().Value(Serein::Menu::kConfirmRepeat);
}

} // namespace Serein::Hooks::Menu
