// Generated from proto/serein/settings/v1/menu.proto by tools/serein/codegen; do not edit.
#pragma once

#include "serein/core/options.h"

namespace Serein::Menu {

[[nodiscard]] bool ValidateConfig(const QByteArray &value);

inline const auto kMenuConfig = Option<QByteArray>{
	"serein.messageMenu",
	Scope::Device,
	QByteArray(),
	Category::Menu,
	"lng_serein_menu",
	0,
	&ValidateConfig };
inline constexpr auto kConfirmRepeat = Option<bool>{
	"serein.confirmRepeat",
	Scope::Device,
	false,
	Category::Menu,
	"lng_serein_menu_confirm_repeat",
	0 };

inline void RegisterOptions(Registry &registry) {
	Expects(registry.Add(kMenuConfig));
	Expects(registry.Add(kConfirmRepeat));
}

} // namespace Serein::Menu
