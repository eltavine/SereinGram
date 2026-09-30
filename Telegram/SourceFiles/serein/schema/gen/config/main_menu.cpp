// Generated from proto/serein/config/v1/main_menu.proto by tools/serein/codegen; do not edit.
#include "serein/schema/gen/config/main_menu.h"

namespace Serein::Interface {

bool Read(
		const QJsonValue &json,
		MainMenuConfig &result,
		Codec::Error &error,
		const QString &path) {
	if (!json.isObject()) {
		return Codec::FailExpected(error, path, "an object");
	}
	const auto object = json.toObject();
	if (!Codec::KnownKeys(object, {
			QLatin1StringView("title"),
			QLatin1StringView("seasonalDecorations"),
			QLatin1StringView("order"),
			QLatin1StringView("hidden"),
		}, error, path)) {
		return false;
	} else if (!Codec::RequiredKeys(object, {
			QLatin1StringView("title"),
			QLatin1StringView("seasonalDecorations"),
			QLatin1StringView("order"),
			QLatin1StringView("hidden"),
		}, error, path)) {
		return false;
	}
	result = MainMenuConfig();
	return true
		&& Codec::ReadField(object, QLatin1StringView("title"), result.title, error, path)
		&& Codec::ReadField(object, QLatin1StringView("seasonalDecorations"), result.seasonalDecorations, error, path)
		&& Codec::ReadField(object, QLatin1StringView("order"), result.order, error, path)
		&& Codec::ReadField(object, QLatin1StringView("hidden"), result.hidden, error, path);
}

QJsonValue Write(const MainMenuConfig &value) {
	auto object = QJsonObject();
	Codec::WriteField(object, QLatin1StringView("title"), value.title);
	Codec::WriteField(object, QLatin1StringView("seasonalDecorations"), value.seasonalDecorations);
	Codec::WriteField(object, QLatin1StringView("order"), value.order);
	Codec::WriteField(object, QLatin1StringView("hidden"), value.hidden);
	return object;
}

bool Validate(
		const MainMenuConfig &value,
		Codec::Error &error,
		const QString &path) {
	if (!(Codec::Unique(value.order))) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("order")), QString::fromLatin1("violates the schema rules"));
	}
	for (auto i = qsizetype(); i != qsizetype(value.order.size()); ++i) {
		const auto &item = value.order[i];
		if (!((item == QString::fromUtf8("profile") || item == QString::fromUtf8("bots") || item == QString::fromUtf8("newGroup") || item == QString::fromUtf8("newChannel") || item == QString::fromUtf8("contacts") || item == QString::fromUtf8("calls") || item == QString::fromUtf8("savedMessages") || item == QString::fromUtf8("settings") || item == QString::fromUtf8("nightMode")))) {
			return Codec::Fail(error, Codec::Item(Codec::Child(path, QLatin1StringView("order")), i), QString::fromLatin1("violates the schema rules"));
		}
	}
	if (!(Codec::Unique(value.hidden))) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("hidden")), QString::fromLatin1("violates the schema rules"));
	}
	for (auto i = qsizetype(); i != qsizetype(value.hidden.size()); ++i) {
		const auto &item = value.hidden[i];
		if (!((item == QString::fromUtf8("profile") || item == QString::fromUtf8("bots") || item == QString::fromUtf8("newGroup") || item == QString::fromUtf8("newChannel") || item == QString::fromUtf8("contacts") || item == QString::fromUtf8("calls") || item == QString::fromUtf8("savedMessages") || item == QString::fromUtf8("nightMode")))) {
			return Codec::Fail(error, Codec::Item(Codec::Child(path, QLatin1StringView("hidden")), i), QString::fromLatin1("violates the schema rules"));
		}
	}
	return true;
}

std::optional<MainMenuConfig> ParseMainMenuConfig(
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
	auto result = MainMenuConfig();
	if (!Read(*object, result, out, QString()) || !Validate(result, out, QString())) {
		return std::nullopt;
	} else if (!ValidMainMenuTitle(result)) {
		Codec::Fail(out, QString(), QString::fromLatin1("violates the document rules"));
		return std::nullopt;
	}
	return result;
}

QByteArray SerializeMainMenuConfig(const MainMenuConfig &value) {
	auto object = Write(value).toObject();
	object.insert(QLatin1StringView("version"), 1);
	return Codec::Serialize(object);
}

} // namespace Serein::Interface
