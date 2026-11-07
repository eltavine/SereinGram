#include "serein/features/presets/model/catalog.h"

#include "base/basic_types.h"

#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QRegularExpression>

#include <algorithm>

namespace Serein::Presets {
namespace {

constexpr auto kVersion = 1;
constexpr auto kMaxPresets = 32;

} // namespace

bool ValidPresetId(const QString &id) {
	static const auto pattern = QRegularExpression(
		u"^[a-z][a-z0-9_]{0,31}$"_q);
	return pattern.match(id).hasMatch();
}

std::optional<std::vector<QString>> ParseCatalog(const QByteArray &json) {
	auto error = QJsonParseError();
	const auto document = QJsonDocument::fromJson(json, &error);
	if (error.error != QJsonParseError::NoError || !document.isObject()) {
		return std::nullopt;
	}
	const auto root = document.object();
	if (root.size() != 2
		|| root.value(u"version"_q) != kVersion
		|| !root.value(u"presets"_q).isArray()) {
		return std::nullopt;
	}
	const auto list = root.value(u"presets"_q).toArray();
	if (list.size() > kMaxPresets) {
		return std::nullopt;
	}
	auto result = std::vector<QString>();
	result.reserve(list.size());
	for (const auto &value : list) {
		const auto id = value.toString();
		if (!value.isString()
			|| !ValidPresetId(id)
			|| std::ranges::find(result, id) != result.end()) {
			return std::nullopt;
		}
		result.push_back(id);
	}
	return result;
}

} // namespace Serein::Presets
