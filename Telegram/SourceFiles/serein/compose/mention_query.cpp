#include "serein/compose/mention_query.h"

#include "base/basic_types.h"

#include <QtCore/QRegularExpression>

namespace Serein::Compose {
namespace {

[[nodiscard]] QString WithoutPrefix(QString value) {
	for (const auto &scheme : { u"https://"_q, u"http://"_q }) {
		if (value.startsWith(scheme, Qt::CaseInsensitive)) {
			value = value.mid(scheme.size());
			break;
		}
	}
	for (const auto &host : { u"t.me/"_q, u"telegram.me/"_q }) {
		if (value.startsWith(host, Qt::CaseInsensitive)) {
			return value.mid(host.size());
		}
	}
	return value.startsWith(u'@') ? value.mid(1) : value;
}

} // namespace

MentionQuery ParseMentionQuery(const QString &input) {
	static const auto kUserLink = QRegularExpression(
		u"^tg://user\\?id=([1-9][0-9]{0,15})$"_q,
		QRegularExpression::CaseInsensitiveOption);
	static const auto kUserId = QRegularExpression(
		u"^[1-9][0-9]{0,15}$"_q);
	static const auto kUsername = QRegularExpression(
		u"^[A-Za-z][A-Za-z0-9_]{3,31}$"_q);
	const auto trimmed = input.trimmed();
	if (const auto match = kUserLink.match(trimmed); match.hasMatch()) {
		return { .userId = match.captured(1).toULongLong() };
	}
	const auto value = WithoutPrefix(trimmed);
	if (kUserId.match(value).hasMatch()) {
		return { .userId = value.toULongLong() };
	} else if (kUsername.match(value).hasMatch()) {
		return { .username = value };
	}
	return {};
}

} // namespace Serein::Compose
