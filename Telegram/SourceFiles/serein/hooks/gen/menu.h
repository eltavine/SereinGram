// Generated from proto/serein/settings/v1/menu.proto by tools/serein/codegen; do not edit.
#pragma once

#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <rpl/producer.h>

namespace Serein::Hooks::Menu {

[[nodiscard]] bool ConfirmRepeat();
[[nodiscard]] rpl::producer<bool> ConfirmRepeatValue();
[[nodiscard]] QString QuickRatingFirst();
[[nodiscard]] rpl::producer<QString> QuickRatingFirstValue();
[[nodiscard]] QString QuickRatingSecond();
[[nodiscard]] rpl::producer<QString> QuickRatingSecondValue();
[[nodiscard]] QByteArray MenuConfig();
[[nodiscard]] rpl::producer<QByteArray> MenuConfigValue();

} // namespace Serein::Hooks::Menu
