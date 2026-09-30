// Generated from proto/serein/config/v1/filters.proto by tools/serein/codegen; do not edit.
#pragma once

#include "serein/schema/codec.h"

namespace Serein::Filters {

struct FilterRule {
	QString id;
	QString title;
	QString pattern;
	QString replacement;
	bool enabled = false;
	bool caseInsensitive = false;
	bool reversed = false;
	QString action;
	std::vector<QString> peers;

	friend bool operator==(const FilterRule &, const FilterRule &) = default;
};

[[nodiscard]] bool Read(
	const QJsonValue &json,
	FilterRule &result,
	Codec::Error &error,
	const QString &path);
[[nodiscard]] QJsonValue Write(const FilterRule &value);
[[nodiscard]] bool Validate(
	const FilterRule &value,
	Codec::Error &error,
	const QString &path);

struct FilterRules {
	bool enabled = false;
	bool filterOutgoing = false;
	bool hideBlocked = false;
	bool stripZalgo = false;
	std::vector<QString> hiddenAuthors;
	std::vector<QString> excludedPeers;
	std::vector<FilterRule> rules;

	friend bool operator==(const FilterRules &, const FilterRules &) = default;
};

[[nodiscard]] bool Read(
	const QJsonValue &json,
	FilterRules &result,
	Codec::Error &error,
	const QString &path);
[[nodiscard]] QJsonValue Write(const FilterRules &value);
[[nodiscard]] bool Validate(
	const FilterRules &value,
	Codec::Error &error,
	const QString &path);
[[nodiscard]] bool ValidFilterRules(const FilterRules &value);
[[nodiscard]] std::optional<FilterRules> ParseFilterRules(
	const QByteArray &raw,
	Codec::Error *error = nullptr);
[[nodiscard]] QByteArray SerializeFilterRules(const FilterRules &value);

struct FilterRuleList {
	std::vector<FilterRule> rules;

	friend bool operator==(const FilterRuleList &, const FilterRuleList &) = default;
};

[[nodiscard]] bool Read(
	const QJsonValue &json,
	FilterRuleList &result,
	Codec::Error &error,
	const QString &path);
[[nodiscard]] QJsonValue Write(const FilterRuleList &value);
[[nodiscard]] bool Validate(
	const FilterRuleList &value,
	Codec::Error &error,
	const QString &path);
[[nodiscard]] std::optional<FilterRuleList> ParseFilterRuleList(
	const QByteArray &raw,
	Codec::Error *error = nullptr);
[[nodiscard]] QByteArray SerializeFilterRuleList(const FilterRuleList &value);

} // namespace Serein::Filters
