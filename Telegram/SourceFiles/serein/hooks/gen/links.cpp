// Generated from proto/serein/settings/v1/links.proto by tools/serein/codegen; do not edit.
#include "serein/hooks/gen/links.h"

#include "serein/core/options.h"
#include "serein/schema/gen/settings/links.h"

namespace Serein::Hooks::Links {

QByteArray Rules() {
	return ForDevice().Get(Serein::Links::kRules);
}

rpl::producer<QByteArray> RulesValue() {
	return ForDevice().Value(Serein::Links::kRules);
}

bool PreviewLinkRules() {
	return ForDevice().Get(Serein::Links::kPreviewLinkRules);
}

rpl::producer<bool> PreviewLinkRulesValue() {
	return ForDevice().Value(Serein::Links::kPreviewLinkRules);
}

} // namespace Serein::Hooks::Links
