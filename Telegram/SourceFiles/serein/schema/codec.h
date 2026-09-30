#pragma once

#include <QtCore/QByteArray>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtCore/QLatin1StringView>
#include <QtCore/QString>

#include <initializer_list>
#include <map>
#include <optional>
#include <vector>

namespace Serein::Codec {

struct Error {
	QString path;
	QString message;
};

bool Fail(Error &error, const QString &path, const QString &message);
bool FailExpected(Error &error, const QString &path, const char *what);

[[nodiscard]] bool Read(const QJsonValue &json, bool &result, Error &error, const QString &path);
[[nodiscard]] bool Read(const QJsonValue &json, int &result, Error &error, const QString &path);
[[nodiscard]] bool Read(const QJsonValue &json, qint64 &result, Error &error, const QString &path);
[[nodiscard]] bool Read(const QJsonValue &json, double &result, Error &error, const QString &path);
[[nodiscard]] bool Read(const QJsonValue &json, QString &result, Error &error, const QString &path);
[[nodiscard]] bool Read(const QJsonValue &json, QByteArray &result, Error &error, const QString &path);

[[nodiscard]] QJsonValue Write(bool value);
[[nodiscard]] QJsonValue Write(int value);
[[nodiscard]] QJsonValue Write(qint64 value);
[[nodiscard]] QJsonValue Write(double value);
[[nodiscard]] QJsonValue Write(const QString &value);
[[nodiscard]] QJsonValue Write(const QByteArray &value);

[[nodiscard]] bool KnownKeys(
	const QJsonObject &object,
	std::initializer_list<QLatin1StringView> keys,
	Error &error,
	const QString &path);
[[nodiscard]] bool RequiredKeys(
	const QJsonObject &object,
	std::initializer_list<QLatin1StringView> keys,
	Error &error,
	const QString &path);
[[nodiscard]] std::optional<QJsonObject> ParseObject(
	const QByteArray &raw,
	Error &error);
[[nodiscard]] QByteArray Serialize(const QJsonObject &object);

[[nodiscard]] bool IsUuid(const QString &value);
[[nodiscard]] bool Matches(const QString &value, const QString &pattern);
[[nodiscard]] QString Child(const QString &path, QLatin1StringView key);
[[nodiscard]] QString Item(const QString &path, qsizetype index);
[[nodiscard]] QString Entry(const QString &path, const QString &key);

template <typename Type>
[[nodiscard]] bool Unique(const std::vector<Type> &values) {
	for (auto i = values.begin(); i != values.end(); ++i) {
		for (auto j = i + 1; j != values.end(); ++j) {
			if (*i == *j) {
				return false;
			}
		}
	}
	return true;
}

template <typename Type>
[[nodiscard]] bool Read(
		const QJsonValue &json,
		std::vector<Type> &result,
		Error &error,
		const QString &path) {
	if (!json.isArray()) {
		return FailExpected(error, path, "an array");
	}
	const auto array = json.toArray();
	result.clear();
	result.reserve(array.size());
	for (auto i = qsizetype(); i != array.size(); ++i) {
		auto item = Type();
		if (!Read(array[i], item, error, Item(path, i))) {
			return false;
		}
		result.push_back(std::move(item));
	}
	return true;
}

template <typename Type>
[[nodiscard]] bool Read(
		const QJsonValue &json,
		std::optional<Type> &result,
		Error &error,
		const QString &path) {
	auto value = Type();
	if (!Read(json, value, error, path)) {
		return false;
	}
	result = std::move(value);
	return true;
}

template <typename Type>
[[nodiscard]] QJsonValue Write(const std::vector<Type> &values) {
	auto array = QJsonArray();
	for (const auto &value : values) {
		array.push_back(Write(value));
	}
	return array;
}

template <typename Type>
[[nodiscard]] bool Read(
		const QJsonValue &json,
		std::map<QString, Type> &result,
		Error &error,
		const QString &path) {
	if (!json.isObject()) {
		return FailExpected(error, path, "an object");
	}
	const auto object = json.toObject();
	result.clear();
	for (auto i = object.begin(); i != object.end(); ++i) {
		auto item = Type();
		if (!Read(i.value(), item, error, Entry(path, i.key()))) {
			return false;
		}
		result.emplace(i.key(), std::move(item));
	}
	return true;
}

template <typename Type>
[[nodiscard]] QJsonValue Write(const std::map<QString, Type> &values) {
	auto object = QJsonObject();
	for (const auto &[key, value] : values) {
		object.insert(key, Write(value));
	}
	return object;
}

template <typename Type>
[[nodiscard]] bool ReadField(
		const QJsonObject &object,
		QLatin1StringView key,
		Type &result,
		Error &error,
		const QString &path) {
	const auto value = object.value(key);
	if (value.isUndefined() || value.isNull()) {
		return true;
	}
	return Read(value, result, error, Child(path, key));
}

template <typename Type>
void WriteField(QJsonObject &object, QLatin1StringView key, const Type &value) {
	object.insert(key, Write(value));
}

template <typename Type>
void WriteField(
		QJsonObject &object,
		QLatin1StringView key,
		const std::optional<Type> &value) {
	if (value) {
		object.insert(key, Write(*value));
	}
}

template <typename Type>
void WriteNullableField(
		QJsonObject &object,
		QLatin1StringView key,
		const std::optional<Type> &value) {
	object.insert(key, value ? Write(*value) : QJsonValue(QJsonValue::Null));
}

} // namespace Serein::Codec
