// Generated from proto/serein/settings/v1/filters.proto by tools/serein/codegen; do not edit.
#include "serein/hooks/gen/filters.h"

#include "serein/core/options.h"
#include "serein/schema/gen/settings/filters.h"

namespace Serein::Hooks::Filters {

QByteArray Rules(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Get(Serein::Filters::kRules);
}

rpl::producer<QByteArray> RulesValue(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Value(Serein::Filters::kRules);
}

QString RuleSubscription() {
	return ForDevice().Get(Serein::Filters::kRuleSubscription);
}

rpl::producer<QString> RuleSubscriptionValue() {
	return ForDevice().Value(Serein::Filters::kRuleSubscription);
}

QByteArray SubscribedRules() {
	return ForDevice().Get(Serein::Filters::kSubscribedRules);
}

rpl::producer<QByteArray> SubscribedRulesValue() {
	return ForDevice().Value(Serein::Filters::kSubscribedRules);
}

QString HiddenMessages(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Get(Serein::Filters::kHiddenMessages);
}

rpl::producer<QString> HiddenMessagesValue(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Value(Serein::Filters::kHiddenMessages);
}

QByteArray KeywordAlerts(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Get(Serein::Filters::kKeywordAlerts);
}

rpl::producer<QByteArray> KeywordAlertsValue(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Value(Serein::Filters::kKeywordAlerts);
}

} // namespace Serein::Hooks::Filters
