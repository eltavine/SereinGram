#pragma once

#include "nagram/core/options.h"

#include <QtCore/QStringList>

namespace Nagram {

struct ExchangeChange {
	QString key;
	QByteArray before;
	QByteArray after;
};

struct ExchangeExport {
	QByteArray data;
	QStringList invalidKeys;
};

struct ExchangePlan {
	std::vector<ExchangeChange> changes;
	QStringList skippedKeys;
	QString error;
};

struct ExchangeApply {
	bool applied = false;
	QString error;
};

class Exchange final {
public:
	[[nodiscard]] static ExchangeExport Export(
		Options &options,
		const Registry &registry);
	[[nodiscard]] static ExchangePlan PlanImport(
		Options &options,
		const Registry &registry,
		const QByteArray &data);
	[[nodiscard]] static ExchangeApply Apply(
		Options &options,
		const Registry &registry,
		const ExchangePlan &plan);
};

} // namespace Nagram
