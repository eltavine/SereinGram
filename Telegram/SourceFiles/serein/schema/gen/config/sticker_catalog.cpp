// Generated from proto/serein/config/v1/sticker_catalog.proto by tools/serein/codegen; do not edit.
#include "serein/schema/gen/config/sticker_catalog.h"

namespace Serein::MediaSchema {

bool Read(
		const QJsonValue &json,
		StickerCatalogSet &result,
		Codec::Error &error,
		const QString &path) {
	if (!json.isObject()) {
		return Codec::FailExpected(error, path, "an object");
	}
	const auto object = json.toObject();
	if (!Codec::KnownKeys(object, {
			QLatin1StringView("shortName"),
			QLatin1StringView("title"),
			QLatin1StringView("type"),
		}, error, path)) {
		return false;
	} else if (!Codec::RequiredKeys(object, {
			QLatin1StringView("shortName"),
			QLatin1StringView("title"),
			QLatin1StringView("type"),
		}, error, path)) {
		return false;
	}
	result = StickerCatalogSet();
	return true
		&& Codec::ReadField(object, QLatin1StringView("shortName"), result.shortName, error, path)
		&& Codec::ReadField(object, QLatin1StringView("title"), result.title, error, path)
		&& Codec::ReadField(object, QLatin1StringView("type"), result.type, error, path);
}

QJsonValue Write(const StickerCatalogSet &value) {
	auto object = QJsonObject();
	Codec::WriteField(object, QLatin1StringView("shortName"), value.shortName);
	Codec::WriteField(object, QLatin1StringView("title"), value.title);
	Codec::WriteField(object, QLatin1StringView("type"), value.type);
	return object;
}

bool Validate(
		const StickerCatalogSet &value,
		Codec::Error &error,
		const QString &path) {
	if (!(Codec::Matches(value.shortName, QString::fromUtf8("^[a-zA-Z0-9_]{1,64}$")))) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("shortName")), QString::fromLatin1("violates the schema rules"));
	}
	if (!(value.title.toUcs4().size() <= 256)) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("title")), QString::fromLatin1("violates the schema rules"));
	}
	if (!((value.type == QString::fromUtf8("stickers") || value.type == QString::fromUtf8("masks") || value.type == QString::fromUtf8("emoji")))) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("type")), QString::fromLatin1("violates the schema rules"));
	}
	return true;
}

bool Read(
		const QJsonValue &json,
		StickerCatalogFile &result,
		Codec::Error &error,
		const QString &path) {
	if (!json.isObject()) {
		return Codec::FailExpected(error, path, "an object");
	}
	const auto object = json.toObject();
	if (!Codec::KnownKeys(object, {
			QLatin1StringView("format"),
			QLatin1StringView("sets"),
		}, error, path)) {
		return false;
	} else if (!Codec::RequiredKeys(object, {
			QLatin1StringView("format"),
			QLatin1StringView("sets"),
		}, error, path)) {
		return false;
	}
	result = StickerCatalogFile();
	return true
		&& Codec::ReadField(object, QLatin1StringView("format"), result.format, error, path)
		&& Codec::ReadField(object, QLatin1StringView("sets"), result.sets, error, path);
}

QJsonValue Write(const StickerCatalogFile &value) {
	auto object = QJsonObject();
	Codec::WriteField(object, QLatin1StringView("format"), value.format);
	Codec::WriteField(object, QLatin1StringView("sets"), value.sets);
	return object;
}

bool Validate(
		const StickerCatalogFile &value,
		Codec::Error &error,
		const QString &path) {
	if (!(value.format == QString::fromUtf8("serein-sticker-catalog"))) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("format")), QString::fromLatin1("violates the schema rules"));
	}
	if (!(qsizetype(value.sets.size()) <= 1000)) {
		return Codec::Fail(error, Codec::Child(path, QLatin1StringView("sets")), QString::fromLatin1("violates the schema rules"));
	}
	for (auto i = qsizetype(); i != qsizetype(value.sets.size()); ++i) {
		const auto &item = value.sets[i];
		if (!Validate(item, error, Codec::Item(Codec::Child(path, QLatin1StringView("sets")), i))) {
			return false;
		}
	}
	return true;
}

std::optional<StickerCatalogFile> ParseStickerCatalogFile(
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
	auto result = StickerCatalogFile();
	if (!Read(*object, result, out, QString()) || !Validate(result, out, QString())) {
		return std::nullopt;
	} else if (!ValidStickerCatalogFile(result)) {
		Codec::Fail(out, QString(), QString::fromLatin1("violates the document rules"));
		return std::nullopt;
	}
	return result;
}

QByteArray SerializeStickerCatalogFile(const StickerCatalogFile &value) {
	auto object = Write(value).toObject();
	object.insert(QLatin1StringView("version"), 1);
	return Codec::Serialize(object);
}

} // namespace Serein::MediaSchema
