// Generated from proto/serein/config/v1/menu.proto by tools/serein/codegen; do not edit.
#include "serein/schema/gen/config/menu.h"

namespace Serein::Menu {

bool Read(
		const QJsonValue &json,
		MenuConfig &result,
		Codec::Error &error,
		const QString &path) {
	if (!json.isObject()) {
		return Codec::FailExpected(error, path, "an object");
	}
	const auto object = json.toObject();
	if (!Codec::KnownKeys(object, {
			QLatin1StringView("states"),
		}, error, path)) {
		return false;
	} else if (!Codec::RequiredKeys(object, {
			QLatin1StringView("states"),
		}, error, path)) {
		return false;
	}
	result = MenuConfig();
	return true
		&& Codec::ReadField(object, QLatin1StringView("states"), result.states, error, path);
}

QJsonValue Write(const MenuConfig &value) {
	auto object = QJsonObject();
	Codec::WriteField(object, QLatin1StringView("states"), value.states);
	return object;
}

bool Validate(
		const MenuConfig &value,
		Codec::Error &error,
		const QString &path) {
	for (const auto &[key, item] : value.states) {
		if (!((item == QString::fromUtf8("show") || item == QString::fromUtf8("hide") || item == QString::fromUtf8("option")))) {
			return Codec::Fail(error, Codec::Entry(Codec::Child(path, QLatin1StringView("states")), key), QString::fromLatin1("violates the schema rules"));
		}
	}
	return true;
}

std::optional<MenuConfig> ParseMenuConfig(
		const QByteArray &raw,
		Codec::Error *error) {
	auto ignored = Codec::Error();
	auto &out = error ? *error : ignored;
	auto object = Codec::ParseObject(raw, out);
	if (!object) {
		return std::nullopt;
	} else if (object->value(QLatin1StringView("version")) != QJsonValue(2)) {
		Codec::Fail(out, QString::fromLatin1("version"), QString::fromLatin1("unsupported version"));
		return std::nullopt;
	}
	object->remove(QLatin1StringView("version"));
	auto result = MenuConfig();
	if (!Read(*object, result, out, QString()) || !Validate(result, out, QString())) {
		return std::nullopt;
	} else if (!ValidMenuConfig(result)) {
		Codec::Fail(out, QString(), QString::fromLatin1("violates the document rules"));
		return std::nullopt;
	}
	return result;
}

QByteArray SerializeMenuConfig(const MenuConfig &value) {
	auto object = Write(value).toObject();
	object.insert(QLatin1StringView("version"), 2);
	return Codec::Serialize(object);
}

} // namespace Serein::Menu
