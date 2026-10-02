#include "serein/network/proxy_notes.h"

#include "serein/core/options.h"
#include "serein/schema/gen/settings/services.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include <algorithm>

namespace Serein::Network {
namespace {

constexpr auto kMaxKeyLength = 300;

[[nodiscard]] bool ValidKey(const QString &key) {
	return !key.isEmpty()
		&& key.size() <= kMaxKeyLength
		&& key == key.toLower()
		&& key.contains(u':');
}

} // namespace

QString ProxyNoteKey(const QString &host, quint32 port) {
	return host.trimmed().toLower() + u':' + QString::number(port);
}

bool ValidProxyNote(const QString &note) {
	return !note.isEmpty()
		&& note.size() <= kMaxProxyNoteLength
		&& note.trimmed() == note
		&& QString::fromUtf8(note.toUtf8()) == note
		&& std::none_of(note.begin(), note.end(), [](QChar ch) {
			return ch.category() == QChar::Other_Control
				|| ch.category() == QChar::Separator_Line
				|| ch.category() == QChar::Separator_Paragraph;
		});
}

std::optional<ProxyNotes> ParseProxyNotes(const QByteArray &raw) {
	if (raw.isEmpty()) {
		return ProxyNotes();
	}
	auto error = QJsonParseError();
	const auto document = QJsonDocument::fromJson(raw, &error);
	if (error.error != QJsonParseError::NoError || !document.isObject()) {
		return std::nullopt;
	}
	const auto object = document.object();
	if (object.isEmpty() || object.size() > kMaxProxyNotes) {
		return std::nullopt;
	}
	auto result = ProxyNotes();
	for (auto i = object.begin(); i != object.end(); ++i) {
		const auto note = i.value().toString();
		if (!i.value().isString() || !ValidKey(i.key()) || !ValidProxyNote(note)) {
			return std::nullopt;
		}
		result.emplace(i.key(), note);
	}
	return result;
}

QByteArray SerializeProxyNotes(const ProxyNotes &notes) {
	if (notes.empty()) {
		return QByteArray();
	}
	auto object = QJsonObject();
	for (const auto &[key, note] : notes) {
		object.insert(key, note);
	}
	return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

} // namespace Serein::Network

namespace Serein {

bool ServiceSettings::ValidProxyNotes(const QByteArray &value) {
	return Network::ParseProxyNotes(value).has_value();
}

} // namespace Serein
