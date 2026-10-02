// Generated from proto/serein/settings/v1/links.proto by tools/serein/codegen; do not edit.
#pragma once

#include "serein/core/options.h"

namespace Serein::Links {

[[nodiscard]] bool Validate(const QByteArray &value);

inline const auto kRules = Option<QByteArray>{
	"serein.linkRules",
	Scope::Device,
	QByteArray(),
	Category::Rules,
	"lng_serein_link_rules",
	0,
	&Validate };
inline constexpr auto kPreviewLinkRules = Option<bool>{
	"serein.previewLinkRules",
	Scope::Device,
	false,
	Category::Rules,
	"lng_serein_link_previews",
	0 };

inline void RegisterOptions(Registry &registry) {
	Expects(registry.Add(kRules));
	Expects(registry.Add(kPreviewLinkRules));
}

} // namespace Serein::Links
