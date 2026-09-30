// Generated from proto/serein/config/v1/menu.proto by tools/serein/codegen; do not edit.
#pragma once

#include "serein/schema/codec.h"

namespace Serein::Menu {

struct MenuConfig {
	std::map<QString, QString> states;

	friend bool operator==(const MenuConfig &, const MenuConfig &) = default;
};

[[nodiscard]] bool Read(
	const QJsonValue &json,
	MenuConfig &result,
	Codec::Error &error,
	const QString &path);
[[nodiscard]] QJsonValue Write(const MenuConfig &value);
[[nodiscard]] bool Validate(
	const MenuConfig &value,
	Codec::Error &error,
	const QString &path);
[[nodiscard]] bool ValidMenuConfig(const MenuConfig &value);
[[nodiscard]] std::optional<MenuConfig> ParseMenuConfig(
	const QByteArray &raw,
	Codec::Error *error = nullptr);
[[nodiscard]] QByteArray SerializeMenuConfig(const MenuConfig &value);

} // namespace Serein::Menu
