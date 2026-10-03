#pragma once

#include "serein/core/options.h"

#include <QtCore/QStringList>

namespace Serein {

struct ExchangeChange {
	QString key;
	QByteArray before;
	QByteArray after;
	Scope scope = Scope::Device;
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
	// A null account keeps the document limited to the device settings.
	[[nodiscard]] static ExchangeExport Export(
		Options &device,
		Options *account,
		const Registry &registry);
	[[nodiscard]] static ExchangePlan PlanImport(
		Options &device,
		Options *account,
		const Registry &registry,
		const QByteArray &data);
	[[nodiscard]] static ExchangePlan PlanReset(
		Options &device,
		Options *account,
		const Registry &registry);
	[[nodiscard]] static ExchangeApply Apply(
		Options &device,
		Options *account,
		const Registry &registry,
		const ExchangePlan &plan);

	[[nodiscard]] static ExchangeExport Export(
		Options &options,
		const Registry &registry);
	[[nodiscard]] static ExchangePlan PlanImport(
		Options &options,
		const Registry &registry,
		const QByteArray &data);
	[[nodiscard]] static ExchangePlan PlanReset(
		Options &options,
		const Registry &registry);
	[[nodiscard]] static ExchangeApply Apply(
		Options &options,
		const Registry &registry,
		const ExchangePlan &plan);

	[[nodiscard]] static bool Transferable(const OptionInfo &info);

private:
	[[nodiscard]] static bool ValidStores(Options &device, Options *account);

};

} // namespace Serein
