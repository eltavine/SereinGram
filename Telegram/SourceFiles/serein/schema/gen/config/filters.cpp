// Generated from proto/serein/config/v1/filters.proto by tools/serein/codegen; do not edit.
#include "serein/schema/gen/config/filters.h"

namespace Serein::Filters {

bool Read(
		const QJsonValue &json,
		FilterRule &result,
		Codec::Error &error,
		const QString &path) {
	if (!json.isObject()) {
		return Codec::FailExpected(error, path, "an object");
	}
	const auto object = json.toObject();
	if (!Codec::KnownKeys(object, {
			QLatin1StringView("id"),
			QLatin1StringView("title"),
			QLatin1StringView("pattern"),
			QLatin1StringView("replacement"),
			QLatin1StringView("enabled"),
			QLatin1StringView("caseInsensitive"),
			QLatin1StringView("reversed"),
			QLatin1StringView("action"),
		}, error, path)) {
		return false;
	} else if (!Codec::RequiredKeys(object, {
			QLatin1StringView("id"),
			QLatin1StringView("title"),
			QLatin1StringView("pattern"),
			QLatin1StringView("replacement"),
			QLatin1StringView("enabled"),
			QLatin1StringView("caseInsensitive"),
			QLatin1StringView("reversed"),
			QLatin1StringView("action"),
		}, error, path)) {
		return false;
	}
	result = FilterRule();
	return true
		&& Codec::ReadField(object, QLatin1StringView("id"), result.id, error, path)
		&& Codec::ReadField(object, QLatin1StringView("title"), result.title, error, path)
		&& Codec::ReadField(object, QLatin1StringView("pattern"), result.pattern, error, path)
		&& Codec::ReadField(object, QLatin1StringView("replacement"), result.replacement, error, path)
		&& Codec::ReadField(object, QLatin1StringView("enabled"), result.enabled, error, path)
		&& Codec::ReadField(object, QLatin1StringView("caseInsensitive"), result.caseInsensitive, error, path)
		&& Codec::ReadField(object, QLatin1StringView("reversed"), result.reversed, error, path)
		&& Codec::ReadField(object, QLatin1StringView("action"), result.action, error, path);
}

QJsonValue Write(const FilterRule &value) {
	auto object = QJsonObject();
	Codec::WriteField(object, QLatin1StringView("id"), value.id);
	Codec::WriteField(object, QLatin1StringView("title"), value.title);
	Codec::WriteField(object, QLatin1StringView("pattern"), value.pattern);
	Codec::WriteField(object, QLatin1StringView("replacement"), value.replacement);
	Codec::WriteField(object, QLatin1StringView("enabled"), value.enabled);
	Codec::WriteField(object, QLatin1StringView("caseInsensitive"), value.caseInsensitive);
	Codec::WriteField(object, QLatin1StringView("reversed"), value.reversed);
	Codec::WriteField(object, QLatin1StringView("action"), value.action);
	return object;
}

bool Validate(
		const FilterRule &value,
		Codec::Error &error,
		const QString &path) {
	if (!(Codec::Matches(value.id, QString::fromUtf8("^[0-9a-f]{8}(-[0-9a-f]{4}){3}-[0-9a-f]{12}$")))) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("id")), QString::fromLatin1("violates the schema rules"));
	}
	if (!((value.action == QString::fromUtf8("mask") || value.action == QString::fromUtf8("replace") || value.action == QString::fromUtf8("hide")))) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("action")), QString::fromLatin1("violates the schema rules"));
	}
	return true;
}

bool Read(
		const QJsonValue &json,
		FilterRules &result,
		Codec::Error &error,
		const QString &path) {
	if (!json.isObject()) {
		return Codec::FailExpected(error, path, "an object");
	}
	const auto object = json.toObject();
	if (!Codec::KnownKeys(object, {
			QLatin1StringView("enabled"),
			QLatin1StringView("filterOutgoing"),
			QLatin1StringView("hideBlocked"),
			QLatin1StringView("stripZalgo"),
			QLatin1StringView("hiddenAuthors"),
			QLatin1StringView("excludedPeers"),
			QLatin1StringView("rules"),
		}, error, path)) {
		return false;
	} else if (!Codec::RequiredKeys(object, {
			QLatin1StringView("enabled"),
			QLatin1StringView("filterOutgoing"),
			QLatin1StringView("hideBlocked"),
			QLatin1StringView("stripZalgo"),
			QLatin1StringView("hiddenAuthors"),
			QLatin1StringView("excludedPeers"),
			QLatin1StringView("rules"),
		}, error, path)) {
		return false;
	}
	result = FilterRules();
	return true
		&& Codec::ReadField(object, QLatin1StringView("enabled"), result.enabled, error, path)
		&& Codec::ReadField(object, QLatin1StringView("filterOutgoing"), result.filterOutgoing, error, path)
		&& Codec::ReadField(object, QLatin1StringView("hideBlocked"), result.hideBlocked, error, path)
		&& Codec::ReadField(object, QLatin1StringView("stripZalgo"), result.stripZalgo, error, path)
		&& Codec::ReadField(object, QLatin1StringView("hiddenAuthors"), result.hiddenAuthors, error, path)
		&& Codec::ReadField(object, QLatin1StringView("excludedPeers"), result.excludedPeers, error, path)
		&& Codec::ReadField(object, QLatin1StringView("rules"), result.rules, error, path);
}

QJsonValue Write(const FilterRules &value) {
	auto object = QJsonObject();
	Codec::WriteField(object, QLatin1StringView("enabled"), value.enabled);
	Codec::WriteField(object, QLatin1StringView("filterOutgoing"), value.filterOutgoing);
	Codec::WriteField(object, QLatin1StringView("hideBlocked"), value.hideBlocked);
	Codec::WriteField(object, QLatin1StringView("stripZalgo"), value.stripZalgo);
	Codec::WriteField(object, QLatin1StringView("hiddenAuthors"), value.hiddenAuthors);
	Codec::WriteField(object, QLatin1StringView("excludedPeers"), value.excludedPeers);
	Codec::WriteField(object, QLatin1StringView("rules"), value.rules);
	return object;
}

bool Validate(
		const FilterRules &value,
		Codec::Error &error,
		const QString &path) {
	if (!(qsizetype(value.hiddenAuthors.size()) <= 1000)) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("hiddenAuthors")), QString::fromLatin1("violates the schema rules"));
	}
	if (!(Codec::Unique(value.hiddenAuthors))) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("hiddenAuthors")), QString::fromLatin1("violates the schema rules"));
	}
	for (auto i = qsizetype(); i != qsizetype(value.hiddenAuthors.size()); ++i) {
		const auto &item = value.hiddenAuthors[i];
		if (!(Codec::Matches(item, QString::fromUtf8("^[1-9][0-9]*$")))) {
			return Codec::Fail(error, Codec::Item(Codec::Child(path, QLatin1StringView("hiddenAuthors")), i), QString::fromLatin1("violates the schema rules"));
		}
	}
	if (!(qsizetype(value.excludedPeers.size()) <= 1000)) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("excludedPeers")), QString::fromLatin1("violates the schema rules"));
	}
	if (!(Codec::Unique(value.excludedPeers))) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("excludedPeers")), QString::fromLatin1("violates the schema rules"));
	}
	for (auto i = qsizetype(); i != qsizetype(value.excludedPeers.size()); ++i) {
		const auto &item = value.excludedPeers[i];
		if (!(Codec::Matches(item, QString::fromUtf8("^[1-9][0-9]*$")))) {
			return Codec::Fail(error, Codec::Item(Codec::Child(path, QLatin1StringView("excludedPeers")), i), QString::fromLatin1("violates the schema rules"));
		}
	}
	if (!(qsizetype(value.rules.size()) <= 32)) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("rules")), QString::fromLatin1("violates the schema rules"));
	}
	for (auto i = qsizetype(); i != qsizetype(value.rules.size()); ++i) {
		const auto &item = value.rules[i];
		if (!Validate(item, error, Codec::Item(Codec::Child(path, QLatin1StringView("rules")), i))) {
			return false;
		}
	}
	return true;
}

std::optional<FilterRules> ParseFilterRules(
		const QByteArray &raw,
		Codec::Error *error) {
	auto ignored = Codec::Error();
	auto &out = error ? *error : ignored;
	auto object = Codec::ParseObject(raw, out);
	if (!object) {
		return std::nullopt;
	} else if (object->value(QLatin1StringView("version")) != QJsonValue(1)) {
		Codec::Fail(out, QString::fromLatin1("version"), QString::fromLatin1("unsupported version"));
		return std::nullopt;
	}
	object->remove(QLatin1StringView("version"));
	auto result = FilterRules();
	if (!Read(*object, result, out, QString()) || !Validate(result, out, QString())) {
		return std::nullopt;
	} else if (!ValidFilterRules(result)) {
		Codec::Fail(out, QString(), QString::fromLatin1("violates the document rules"));
		return std::nullopt;
	}
	return result;
}

QByteArray SerializeFilterRules(const FilterRules &value) {
	auto object = Write(value).toObject();
	object.insert(QLatin1StringView("version"), 1);
	return Codec::Serialize(object);
}

} // namespace Serein::Filters
