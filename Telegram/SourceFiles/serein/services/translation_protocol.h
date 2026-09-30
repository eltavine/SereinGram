#pragma once

#include "serein/hooks/services/model.h"

#include <QtCore/QStringList>
#include <QtCore/QUrlQuery>

#include <optional>

namespace Serein {

struct TranslationCall {
	QJsonObject json;
	QUrlQuery query;
	std::optional<QByteArray> form;
};

[[nodiscard]] int TranslationBatchLimit(const ServiceDefinition &service);
[[nodiscard]] TranslationCall BuildTranslationCall(
	const ServiceDefinition &service,
	const QStringList &texts,
	const QString &to);
[[nodiscard]] std::optional<QStringList> ParseTranslationResponse(
	const ServiceDefinition &service,
	const QByteArray &body,
	int expected);

} // namespace Serein
