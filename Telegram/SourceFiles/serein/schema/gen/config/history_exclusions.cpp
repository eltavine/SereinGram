// Generated from proto/serein/config/v1/history_exclusions.proto by tools/serein/codegen; do not edit.
#include "serein/schema/gen/config/history_exclusions.h"

namespace Serein::HistoryFeature {

bool Read(
		const QJsonValue &json,
		HistoryExclusions &result,
		Codec::Error &error,
		const QString &path) {
	if (!json.isObject()) {
		return Codec::FailExpected(error, path, "an object");
	}
	const auto object = json.toObject();
	if (!Codec::KnownKeys(object, {
			QLatin1StringView("peers"),
		}, error, path)) {
		return false;
	}
	result = HistoryExclusions();
	return true
		&& Codec::ReadField(object, QLatin1StringView("peers"), result.peers, error, path);
}

QJsonValue Write(const HistoryExclusions &value) {
	auto object = QJsonObject();
	Codec::WriteField(object, QLatin1StringView("peers"), value.peers);
	return object;
}

bool Validate(
		const HistoryExclusions &value,
		Codec::Error &error,
		const QString &path) {
	if (!(qsizetype(value.peers.size()) <= 10000)) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("peers")), QString::fromLatin1("violates the schema rules"));
	}
	if (!(Codec::Unique(value.peers))) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("peers")), QString::fromLatin1("violates the schema rules"));
	}
	for (auto i = qsizetype(); i != qsizetype(value.peers.size()); ++i) {
		const auto &item = value.peers[i];
		if (!(Codec::Matches(item, QString::fromUtf8("^[1-9][0-9]*$")))) {
			return Codec::Fail(error, Codec::Item(Codec::Child(path, QLatin1StringView("peers")), i), QString::fromLatin1("violates the schema rules"));
		}
	}
	return true;
}

std::optional<HistoryExclusions> ParseHistoryExclusions(
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
	auto result = HistoryExclusions();
	if (!Read(*object, result, out, QString()) || !Validate(result, out, QString())) {
		return std::nullopt;
	}
	return result;
}

QByteArray SerializeHistoryExclusions(const HistoryExclusions &value) {
	auto object = Write(value).toObject();
	object.insert(QLatin1StringView("version"), 1);
	return Codec::Serialize(object);
}

} // namespace Serein::HistoryFeature
