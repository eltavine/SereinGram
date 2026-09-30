#include "serein/schema/codec.h"

#include "base/basic_types.h"

#include <QtCore/QHash>
#include <QtCore/QJsonDocument>
#include <QtCore/QRegularExpression>

#include <algorithm>
#include <cmath>
#include <limits>

namespace Serein::Codec {

bool Fail(Error &error, const QString &path, const QString &message) {
	error.path = path.isEmpty() ? u"$"_q : path;
	error.message = message;
	return false;
}

bool FailExpected(Error &error, const QString &path, const char *what) {
	return Fail(error, path, u"expected "_q + QString::fromLatin1(what));
}

bool Read(const QJsonValue &json, bool &result, Error &error, const QString &path) {
	if (!json.isBool()) {
		return FailExpected(error, path, "a boolean");
	}
	result = json.toBool();
	return true;
}

bool Read(const QJsonValue &json, int &result, Error &error, const QString &path) {
	const auto value = json.toDouble();
	if (!json.isDouble()
		|| std::trunc(value) != value
		|| value < std::numeric_limits<int>::min()
		|| value > std::numeric_limits<int>::max()) {
		return FailExpected(error, path, "a 32-bit integer");
	}
	result = int(value);
	return true;
}

bool Read(const QJsonValue &json, qint64 &result, Error &error, const QString &path) {
	if (json.isString()) {
		auto ok = false;
		const auto value = json.toString().toLongLong(&ok);
		if (ok && QString::number(value) == json.toString()) {
			result = value;
			return true;
		}
	} else if (json.isDouble()) {
		const auto value = json.toDouble();
		if (std::trunc(value) == value && std::abs(value) <= 9007199254740991.) {
			result = qint64(value);
			return true;
		}
	}
	return FailExpected(error, path, "a 64-bit integer");
}

bool Read(const QJsonValue &json, QString &result, Error &error, const QString &path) {
	if (!json.isString()) {
		return FailExpected(error, path, "a string");
	}
	result = json.toString();
	return true;
}

bool Read(const QJsonValue &json, QByteArray &result, Error &error, const QString &path) {
	auto decoded = QByteArray::fromBase64Encoding(
		json.toString().toLatin1(),
		QByteArray::AbortOnBase64DecodingErrors);
	if (!json.isString() || !decoded) {
		return FailExpected(error, path, "a base64 string");
	}
	result = std::move(*decoded);
	return true;
}

QJsonValue Write(bool value) {
	return value;
}

QJsonValue Write(int value) {
	return value;
}

QJsonValue Write(qint64 value) {
	return QString::number(value);
}

QJsonValue Write(const QString &value) {
	return value;
}

QJsonValue Write(const QByteArray &value) {
	return QString::fromLatin1(value.toBase64());
}

bool KnownKeys(
		const QJsonObject &object,
		std::initializer_list<QLatin1StringView> keys,
		Error &error,
		const QString &path) {
	for (auto i = object.begin(); i != object.end(); ++i) {
		if (std::find(keys.begin(), keys.end(), i.key()) == keys.end()) {
			return Fail(error, path, u"unknown field "_q + i.key());
		}
	}
	return true;
}

bool RequiredKeys(
		const QJsonObject &object,
		std::initializer_list<QLatin1StringView> keys,
		Error &error,
		const QString &path) {
	for (const auto &key : keys) {
		if (!object.contains(key)) {
			return Fail(error, path, u"missing field "_q + QString(key));
		}
	}
	return true;
}

std::optional<QJsonObject> ParseObject(const QByteArray &raw, Error &error) {
	auto parseError = QJsonParseError();
	const auto document = QJsonDocument::fromJson(raw, &parseError);
	if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
		FailExpected(error, QString(), "a JSON object");
		return std::nullopt;
	}
	return document.object();
}

QByteArray Serialize(const QJsonObject &object) {
	return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

bool IsUuid(const QString &value) {
	static const auto pattern = QRegularExpression(
		u"\\A[0-9a-fA-F]{8}(?:-[0-9a-fA-F]{4}){3}-[0-9a-fA-F]{12}\\z"_q);
	return pattern.match(value).hasMatch();
}

namespace {

[[nodiscard]] QString EndAnchorsAsRe2(const QString &pattern) {
	auto result = QString();
	result.reserve(pattern.size() + 4);
	auto escaped = false;
	auto inClass = false;
	for (const auto ch : pattern) {
		if (escaped) {
			escaped = false;
		} else if (ch == u'\\') {
			escaped = true;
		} else if (inClass) {
			inClass = (ch != u']');
		} else if (ch == u'[') {
			inClass = true;
		} else if (ch == u'$') {
			result += u"\\z"_q;
			continue;
		}
		result += ch;
	}
	return result;
}

} // namespace

bool Matches(const QString &value, const QString &pattern) {
	thread_local auto cache = QHash<QString, QRegularExpression>();
	auto i = cache.find(pattern);
	if (i == cache.end()) {
		i = cache.insert(pattern, QRegularExpression(EndAnchorsAsRe2(pattern)));
	}
	return i->match(value).hasMatch();
}

QString Child(const QString &path, QLatin1StringView key) {
	return path.isEmpty() ? QString(key) : (path + u'.' + key);
}

QString Item(const QString &path, qsizetype index) {
	return path + u'[' + QString::number(index) + u']';
}

QString Entry(const QString &path, const QString &key) {
	return path + u"[\""_q + key + u"\"]"_q;
}

} // namespace Serein::Codec
