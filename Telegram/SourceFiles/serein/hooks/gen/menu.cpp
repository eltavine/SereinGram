// Generated from proto/serein/settings/v1/menu.proto by tools/serein/codegen; do not edit.
#include "serein/hooks/gen/menu.h"

#include "serein/core/options.h"
#include "serein/schema/gen/settings/menu.h"

namespace Serein::Hooks::Menu {

bool ConfirmRepeat() {
	return ForDevice().Get(Serein::Menu::kConfirmRepeat);
}

rpl::producer<bool> ConfirmRepeatValue() {
	return ForDevice().Value(Serein::Menu::kConfirmRepeat);
}

QString QuickRatingFirst() {
	return ForDevice().Get(Serein::Menu::kQuickRatingFirst);
}

rpl::producer<QString> QuickRatingFirstValue() {
	return ForDevice().Value(Serein::Menu::kQuickRatingFirst);
}

QString QuickRatingSecond() {
	return ForDevice().Get(Serein::Menu::kQuickRatingSecond);
}

rpl::producer<QString> QuickRatingSecondValue() {
	return ForDevice().Value(Serein::Menu::kQuickRatingSecond);
}

QByteArray MenuConfig() {
	return ForDevice().Get(Serein::Menu::kMenuConfig);
}

rpl::producer<QByteArray> MenuConfigValue() {
	return ForDevice().Value(Serein::Menu::kMenuConfig);
}

} // namespace Serein::Hooks::Menu
