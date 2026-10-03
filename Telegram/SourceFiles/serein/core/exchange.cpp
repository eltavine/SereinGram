#include "serein/core/exchange.h"

#include "base/basic_types.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

namespace Serein {
namespace {

constexpr auto kDeviceVersion = 1;
constexpr auto kAccountVersion = 2;
constexpr auto kMaxImportBytes = 8 * 1024 * 1024;

std::optional<QJsonValue> Decode(
		const OptionInfo &info,
		const QByteArray &raw) {
	if (raw.isEmpty()) {
		return std::nullopt;
	}
	auto value = QJsonValue();
	switch (info.type) {
	case OptionInfo::ValueType::Boolean:
		if (raw != "1" && raw != "0") {
			return std::nullopt;
		}
		value = (raw == "1");
		break;
	case OptionInfo::ValueType::Integer: {
		auto ok = false;
		const auto number = raw.toInt(&ok);
		if (!ok || QByteArray::number(number) != raw) {
			return std::nullopt;
		}
		value = number;
		break;
	}
	case OptionInfo::ValueType::String: {
		if (!raw.startsWith('s')) {
			return std::nullopt;
		}
		const auto bytes = raw.mid(1);
		const auto decoded = QString::fromUtf8(bytes);
		if (decoded.toUtf8() != bytes) {
			return std::nullopt;
		}
		value = decoded;
		break;
	}
	case OptionInfo::ValueType::Object: {
		auto error = QJsonParseError();
		const auto document = QJsonDocument::fromJson(raw, &error);
		if (error.error != QJsonParseError::NoError || !document.isObject()) {
			return std::nullopt;
		}
		value = document.object();
		break;
	}
	}
	return info.accepts(value) ? std::optional<QJsonValue>(value) : std::nullopt;
}

QByteArray Encode(const OptionInfo &info, const QJsonValue &value) {
	auto raw = QByteArray();
	switch (info.type) {
	case OptionInfo::ValueType::Boolean:
		raw = value.toBool() ? "1" : "0";
		break;
	case OptionInfo::ValueType::Integer:
		raw = QByteArray::number(value.toInt());
		break;
	case OptionInfo::ValueType::String:
		raw = "s" + value.toString().toUtf8();
		break;
	case OptionInfo::ValueType::Object:
		raw = QJsonDocument(value.toObject()).toJson(QJsonDocument::Compact);
		break;
	}
	return raw == info.fallbackRaw ? QByteArray() : raw;
}

bool Exportable(const OptionInfo &info, Scope scope) {
	if (info.scope != scope) {
		return false;
	}
	return (scope == Scope::Device)
		? (info.flags & static_cast<unsigned>(Flag::Exportable)) != 0
		: (info.flags & static_cast<unsigned>(Flag::Hidden)) == 0;
}

const OptionInfo *Find(const Registry &registry, const QString &key) {
	const auto encoded = key.toUtf8();
	return registry.Find(std::string_view(encoded.constData(), encoded.size()));
}

QString KeyOf(const OptionInfo &info) {
	return QString::fromUtf8(info.key.data(), info.key.size());
}

bool SupportedRoot(const QJsonObject &root) {
	const auto version = root.value("version");
	if (!version.isDouble() || !root.value("options").isObject()) {
		return false;
	}
	const auto number = version.toDouble();
	if (number == kDeviceVersion) {
		return root.size() == 2;
	} else if (number != kAccountVersion) {
		return false;
	}
	for (auto i = root.begin(); i != root.end(); ++i) {
		const auto known = (i.key() == u"version"_q)
			|| (i.key() == u"options"_q)
			|| (i.key() == u"account"_q && i.value().isObject());
		if (!known) {
			return false;
		}
	}
	return true;
}

} // namespace

bool Exchange::Transferable(const OptionInfo &info) {
	return Exportable(info, info.scope);
}

bool Exchange::ValidStores(Options &device, Options *account) {
	return (device._scope == Scope::Device)
		&& (!account || account->_scope == Scope::Account);
}

ExchangeExport Exchange::Export(
		Options &device,
		Options *account,
		const Registry &registry) {
	auto result = ExchangeExport();
	if (!ValidStores(device, account)) {
		return result;
	}
	const auto collect = [&](Options &options, Scope scope) {
		auto values = QJsonObject();
		for (const auto &info : registry.All()) {
			if (!Exportable(info, scope)) {
				continue;
			}
			const auto raw = options._prefs.read(info.key);
			if (raw.isEmpty()) {
				continue;
			}
			if (const auto value = Decode(info, raw)) {
				values.insert(KeyOf(info), *value);
			} else {
				result.invalidKeys.push_back(KeyOf(info));
			}
		}
		return values;
	};
	auto root = QJsonObject();
	root.insert(u"options"_q, collect(device, Scope::Device));
	const auto accountValues = account
		? collect(*account, Scope::Account)
		: QJsonObject();
	if (accountValues.isEmpty()) {
		root.insert(u"version"_q, kDeviceVersion);
	} else {
		root.insert(u"version"_q, kAccountVersion);
		root.insert(u"account"_q, accountValues);
	}
	result.data = QJsonDocument(root).toJson(QJsonDocument::Indented);
	return result;
}

ExchangePlan Exchange::PlanImport(
		Options &device,
		Options *account,
		const Registry &registry,
		const QByteArray &data) {
	auto result = ExchangePlan();
	if (!ValidStores(device, account)) {
		result.error = u"Only device and account settings can be imported."_q;
		return result;
	}
	if (data.size() > kMaxImportBytes) {
		result.error = u"The settings file is too large."_q;
		return result;
	}
	auto parseError = QJsonParseError();
	const auto document = QJsonDocument::fromJson(data, &parseError);
	if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
		result.error = u"The settings file is not valid JSON."_q;
		return result;
	}
	const auto root = document.object();
	if (!SupportedRoot(root)) {
		result.error = u"The settings file has an unsupported schema."_q;
		return result;
	}
	const auto plan = [&](const QJsonObject &values, Options *options, Scope scope) {
		for (auto it = values.begin(); it != values.end(); ++it) {
			const auto info = Find(registry, it.key());
			if (!options || !info || !Exportable(*info, scope)) {
				result.skippedKeys.push_back(it.key());
				continue;
			}
			if (!info->accepts(it.value())) {
				result.error = u"Invalid setting value: "_q + it.key();
				result.changes.clear();
				return false;
			}
			const auto after = Encode(*info, it.value());
			const auto before = options->_prefs.read(info->key);
			if (before != after) {
				result.changes.push_back({ it.key(), before, after, scope });
			}
		}
		return true;
	};
	if (plan(root.value("options").toObject(), &device, Scope::Device)) {
		plan(root.value("account").toObject(), account, Scope::Account);
	}
	return result;
}

ExchangePlan Exchange::PlanReset(
		Options &device,
		Options *account,
		const Registry &registry) {
	auto result = ExchangePlan();
	if (!ValidStores(device, account)) {
		result.error = u"Only device and account settings can be reset."_q;
		return result;
	}
	const auto reset = [&](Options &options, Scope scope) {
		for (const auto &info : registry.All()) {
			if (!Exportable(info, scope)) {
				continue;
			}
			const auto raw = options._prefs.read(info.key);
			if (!raw.isEmpty()) {
				result.changes.push_back({
					.key = KeyOf(info),
					.before = raw,
					.scope = scope,
				});
			}
		}
	};
	reset(device, Scope::Device);
	if (account) {
		reset(*account, Scope::Account);
	}
	return result;
}

ExchangeApply Exchange::Apply(
		Options &device,
		Options *account,
		const Registry &registry,
		const ExchangePlan &plan) {
	if (!ValidStores(device, account) || !plan.error.isEmpty()) {
		return { false, u"The import plan is invalid."_q };
	}
	const auto store = [&](Scope scope) {
		return (scope == Scope::Device) ? &device : account;
	};
	for (const auto &change : plan.changes) {
		const auto info = Find(registry, change.key);
		const auto options = store(change.scope);
		if (!options
			|| !info
			|| !Exportable(*info, change.scope)
			|| options->_prefs.read(info->key) != change.before
			|| (!change.after.isEmpty() && !Decode(*info, change.after))) {
			return { false, u"The settings changed since preview: "_q
				+ change.key };
		}
	}
	for (const auto &change : plan.changes) {
		const auto info = Find(registry, change.key);
		const auto options = store(change.scope);
		if (change.after.isEmpty()) {
			options->_prefs.clear(info->key);
		} else {
			options->_prefs.write(info->key, change.after);
		}
		options->_cache.erase(info->key);
		options->_invalidKeys.erase(info->key);
	}
	for (const auto &change : plan.changes) {
		const auto info = Find(registry, change.key);
		store(change.scope)->_changes.fire_copy(info->key);
	}
	return { true, {} };
}

ExchangeExport Exchange::Export(Options &options, const Registry &registry) {
	return Export(options, nullptr, registry);
}

ExchangePlan Exchange::PlanImport(
		Options &options,
		const Registry &registry,
		const QByteArray &data) {
	return PlanImport(options, nullptr, registry, data);
}

ExchangePlan Exchange::PlanReset(Options &options, const Registry &registry) {
	return PlanReset(options, nullptr, registry);
}

ExchangeApply Exchange::Apply(
		Options &options,
		const Registry &registry,
		const ExchangePlan &plan) {
	return Apply(options, nullptr, registry, plan);
}

} // namespace Serein
