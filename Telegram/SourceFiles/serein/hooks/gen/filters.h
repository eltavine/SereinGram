// Generated from proto/serein/settings/v1/filters.proto by tools/serein/codegen; do not edit.
#pragma once

#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <gsl/pointers>
#include <rpl/producer.h>

namespace Main {
class Session;
} // namespace Main

namespace Serein::Hooks::Filters {

[[nodiscard]] QByteArray Rules(gsl::not_null<Main::Session*> session);
[[nodiscard]] rpl::producer<QByteArray> RulesValue(gsl::not_null<Main::Session*> session);
[[nodiscard]] QString RuleSubscription();
[[nodiscard]] rpl::producer<QString> RuleSubscriptionValue();
[[nodiscard]] QByteArray SubscribedRules();
[[nodiscard]] rpl::producer<QByteArray> SubscribedRulesValue();

} // namespace Serein::Hooks::Filters
