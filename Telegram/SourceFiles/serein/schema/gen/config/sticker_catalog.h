// Generated from proto/serein/config/v1/sticker_catalog.proto by tools/serein/codegen; do not edit.
#pragma once

#include "serein/schema/codec.h"

namespace Serein::MediaSchema {

struct StickerCatalogSet {
	QString shortName;
	QString title;
	QString type;

	friend bool operator==(const StickerCatalogSet &, const StickerCatalogSet &) = default;
};

[[nodiscard]] bool Read(
	const QJsonValue &json,
	StickerCatalogSet &result,
	Codec::Error &error,
	const QString &path);
[[nodiscard]] QJsonValue Write(const StickerCatalogSet &value);
[[nodiscard]] bool Validate(
	const StickerCatalogSet &value,
	Codec::Error &error,
	const QString &path);

struct StickerCatalogFile {
	QString format;
	std::vector<StickerCatalogSet> sets;

	friend bool operator==(const StickerCatalogFile &, const StickerCatalogFile &) = default;
};

[[nodiscard]] bool Read(
	const QJsonValue &json,
	StickerCatalogFile &result,
	Codec::Error &error,
	const QString &path);
[[nodiscard]] QJsonValue Write(const StickerCatalogFile &value);
[[nodiscard]] bool Validate(
	const StickerCatalogFile &value,
	Codec::Error &error,
	const QString &path);
[[nodiscard]] bool ValidStickerCatalogFile(const StickerCatalogFile &value);
[[nodiscard]] std::optional<StickerCatalogFile> ParseStickerCatalogFile(
	const QByteArray &raw,
	Codec::Error *error = nullptr);
[[nodiscard]] QByteArray SerializeStickerCatalogFile(const StickerCatalogFile &value);

} // namespace Serein::MediaSchema
