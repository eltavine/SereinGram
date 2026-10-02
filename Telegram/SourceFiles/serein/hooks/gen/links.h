// Generated from proto/serein/settings/v1/links.proto by tools/serein/codegen; do not edit.
#pragma once

#include <QtCore/QByteArray>
#include <rpl/producer.h>

namespace Serein::Hooks::Links {

[[nodiscard]] QByteArray Rules();
[[nodiscard]] rpl::producer<QByteArray> RulesValue();
[[nodiscard]] bool PreviewLinkRules();
[[nodiscard]] rpl::producer<bool> PreviewLinkRulesValue();

} // namespace Serein::Hooks::Links
