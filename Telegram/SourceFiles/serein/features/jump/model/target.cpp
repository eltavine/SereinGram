#include "serein/features/jump/model/target.h"

#include "base/basic_types.h"

#include <QtCore/QRegularExpression>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QUrlQuery>

#include <limits>

namespace Serein::Jump {
namespace {

constexpr auto kMaxMessageId = qint64(std::numeric_limits<int>::max());

[[nodiscard]] qint64 Positive(QStringView text, qint64 maximum) {
	auto ok = false;
	const auto value = text.toLongLong(&ok);
	return (ok && value > 0 && value <= maximum) ? value : 0;
}

[[nodiscard]] qint64 MessageId(QStringView text) {
	return Positive(text, kMaxMessageId);
}

[[nodiscard]] qint64 ChannelId(QStringView text) {
	return Positive(text, std::numeric_limits<qint64>::max());
}

[[nodiscard]] bool ValidUsername(const QString &name) {
	static const auto kPattern = QRegularExpression(
		u"^[A-Za-z][A-Za-z0-9_]{3,31}$"_q);
	return kPattern.match(name).hasMatch();
}

[[nodiscard]] std::optional<Target> FromParts(QStringList parts) {
	if (!parts.isEmpty() && parts.front() == u"s"_q) {
		parts.pop_front();
	}
	if (parts.size() < 2) {
		return std::nullopt;
	}
	auto result = Target{ .messageId = MessageId(parts.back()) };
	if (!result.messageId) {
		return std::nullopt;
	} else if (parts.front() == u"c"_q) {
		result.channelId = (parts.size() <= 4) ? ChannelId(parts[1]) : 0;
		return (parts.size() >= 3 && result.channelId)
			? std::make_optional(result)
			: std::nullopt;
	}
	result.username = parts.front();
	return (parts.size() <= 3 && ValidUsername(result.username))
		? std::make_optional(result)
		: std::nullopt;
}

[[nodiscard]] std::optional<Target> FromDeepLink(const QUrl &url) {
	const auto query = QUrlQuery(url);
	auto result = Target{
		.messageId = MessageId(query.queryItemValue(u"post"_q)),
	};
	const auto host = url.host().toLower();
	if (!result.messageId) {
		return std::nullopt;
	} else if (host == u"privatepost"_q) {
		result.channelId = ChannelId(query.queryItemValue(u"channel"_q));
		return result.channelId ? std::make_optional(result) : std::nullopt;
	} else if (host == u"resolve"_q) {
		result.username = query.queryItemValue(u"domain"_q);
		return ValidUsername(result.username)
			? std::make_optional(result)
			: std::nullopt;
	}
	return std::nullopt;
}

} // namespace

std::optional<Target> ParseTarget(QStringView input) {
	auto text = input.trimmed().toString();
	if (text.startsWith(u'#')) {
		text = text.mid(1);
	}
	if (const auto id = MessageId(text)) {
		return Target{ .messageId = id };
	}
	const auto url = QUrl::fromUserInput(text);
	const auto scheme = url.scheme().toLower();
	const auto web = (scheme == u"https"_q) || (scheme == u"http"_q);
	if (!url.isValid() || (!web && scheme != u"tg"_q)) {
		return std::nullopt;
	} else if (!web) {
		return FromDeepLink(url);
	}
	const auto host = url.host().toLower();
	auto parts = url.path().split(u'/', Qt::SkipEmptyParts);
	if (host.endsWith(u".t.me"_q)) {
		parts.push_front(host.chopped(5));
	} else if (host != u"t.me"_q
		&& host != u"telegram.me"_q
		&& host != u"telegram.dog"_q) {
		return std::nullopt;
	}
	return FromParts(std::move(parts));
}

} // namespace Serein::Jump
