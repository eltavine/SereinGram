// Generated from proto/serein/settings/v1/menu.proto by tools/serein/codegen; do not edit.
#pragma once

#include <QtCore/QByteArray>
#include <rpl/producer.h>

namespace Serein::Hooks::Menu {

[[nodiscard]] QByteArray MenuConfig();
[[nodiscard]] rpl::producer<QByteArray> MenuConfigValue();
[[nodiscard]] bool ConfirmRepeat();
[[nodiscard]] rpl::producer<bool> ConfirmRepeatValue();

} // namespace Serein::Hooks::Menu
