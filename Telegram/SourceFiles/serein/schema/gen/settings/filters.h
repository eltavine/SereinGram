// Generated from proto/serein/settings/v1/filters.proto by tools/serein/codegen; do not edit.
#pragma once

#include "serein/core/options.h"
#include "serein/schema/codec.h"

namespace Serein::Filters {

[[nodiscard]] bool Validate(const QByteArray &value);
[[nodiscard]] bool ValidRuleList(const QByteArray &value);
[[nodiscard]] bool ValidHiddenMessages(const QString &value);

inline const auto kRules = Option<QByteArray>{
	"serein.filters",
	Scope::Account,
	QByteArray(),
	Category::Rules,
	"lng_serein_filter_rules",
	static_cast<unsigned>(Flag::RefreshMessageView),
	&Validate };
inline const auto kRuleSubscription = Option<QString>{
	"serein.ruleSubscription",
	Scope::Device,
	QString(),
	Category::Rules,
	"lng_serein_filter_subscription",
	0,
	[](const QString &value) {
		return (value == QString())
			|| ((value.toUcs4().size() <= 2048) && (Codec::Matches(value, QString::fromUtf8("^(https://[^\\s]+)?$"))));
	} };
inline const auto kSubscribedRules = Option<QByteArray>{
	"serein.subscribedRules",
	Scope::Device,
	QByteArray(),
	Category::Rules,
	"lng_serein_filter_subscription",
	static_cast<unsigned>(Flag::RefreshMessageView) | static_cast<unsigned>(Flag::Hidden),
	&ValidRuleList };
inline const auto kHiddenMessages = Option<QString>{
	"serein.hiddenMessages",
	Scope::Account,
	QString(),
	Category::Rules,
	"lng_serein_hidden_messages",
	static_cast<unsigned>(Flag::RefreshMessageView),
	&ValidHiddenMessages };

inline void RegisterOptions(Registry &registry) {
	Expects(registry.Add(kRules));
	Expects(registry.Add(kRuleSubscription));
	Expects(registry.Add(kSubscribedRules));
	Expects(registry.Add(kHiddenMessages));
}

} // namespace Serein::Filters
