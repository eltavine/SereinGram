#include "serein/filters/hidden_messages.h"

#include "serein/schema/gen/settings/filters.h"

#include <QtCore/QStringList>

namespace Serein::Filters {
namespace {

[[nodiscard]] bool ValidToken(const QString &token) {
	const auto colon = token.indexOf(u':');
	if (colon <= 0 || colon + 1 >= token.size()) {
		return false;
	}
	auto peerOk = false;
	auto msgOk = false;
	const auto peer = token.left(colon).toULongLong(&peerOk);
	const auto msg = token.mid(colon + 1).toLongLong(&msgOk);
	return peerOk && msgOk && peer && (msg > 0)
		&& (HiddenMessageToken(peer, msg) == token);
}

} // namespace

QString HiddenMessageToken(quint64 peer, qint64 msg) {
	return QString::number(peer) + u':' + QString::number(msg);
}

bool ValidHiddenMessages(const QString &value) {
	if (value.isEmpty()) {
		return true;
	}
	const auto tokens = value.split(u',');
	if (tokens.size() > kHiddenMessagesLimit) {
		return false;
	}
	auto seen = QSet<QString>();
	for (const auto &token : tokens) {
		if (!ValidToken(token) || seen.contains(token)) {
			return false;
		}
		seen.insert(token);
	}
	return true;
}

QSet<QString> HiddenMessageSet(const QString &raw) {
	const auto tokens = raw.split(u',', Qt::SkipEmptyParts);
	return QSet<QString>(tokens.begin(), tokens.end());
}

QString ToggleHiddenMessages(const QString &raw, const QStringList &tokens) {
	auto list = raw.split(u',', Qt::SkipEmptyParts);
	const auto hidden = !tokens.isEmpty() && list.contains(tokens.front());
	for (const auto &token : tokens) {
		list.removeAll(token);
		if (!hidden) {
			list.push_back(token);
		}
	}
	while (list.size() > kHiddenMessagesLimit) {
		list.removeFirst();
	}
	return list.join(u',');
}

} // namespace Serein::Filters
