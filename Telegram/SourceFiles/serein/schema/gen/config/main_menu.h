// Generated from proto/serein/config/v1/main_menu.proto by tools/serein/codegen; do not edit.
#pragma once

#include "serein/schema/codec.h"

namespace Serein::Interface {

struct MainMenuConfig {
	QString title;
	bool seasonalDecorations = false;
	std::vector<QString> order;
	std::vector<QString> hidden;

	friend bool operator==(const MainMenuConfig &, const MainMenuConfig &) = default;
};

[[nodiscard]] bool Read(
	const QJsonValue &json,
	MainMenuConfig &result,
	Codec::Error &error,
	const QString &path);
[[nodiscard]] QJsonValue Write(const MainMenuConfig &value);
[[nodiscard]] bool Validate(
	const MainMenuConfig &value,
	Codec::Error &error,
	const QString &path);
[[nodiscard]] bool ValidMainMenuTitle(const MainMenuConfig &value);
[[nodiscard]] std::optional<MainMenuConfig> ParseMainMenuConfig(
	const QByteArray &raw,
	Codec::Error *error = nullptr);
[[nodiscard]] QByteArray SerializeMainMenuConfig(const MainMenuConfig &value);

} // namespace Serein::Interface
