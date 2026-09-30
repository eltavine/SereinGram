#include "serein/network/proxy_subscription.h"

#include "base/basic_types.h"

#include <QtCore/QRegularExpression>

namespace Serein::Network {

QStringList ExtractProxyLinks(const QString &body) {
	static const auto kLink = QRegularExpression(
		u"(?:tg://|(?:https?://)?(?:t|telegram)\\.me/)(?:proxy|socks)\\?"
		"[^\\s\"'<>`]+"_q,
		QRegularExpression::CaseInsensitiveOption);
	auto result = QStringList();
	auto matches = kLink.globalMatch(body);
	while (matches.hasNext() && result.size() < kSubscriptionLinksLimit) {
		auto link = matches.next().captured(0);
		while (link.endsWith(u',') || link.endsWith(u';')) {
			link.chop(1);
		}
		if (!result.contains(link)) {
			result.push_back(link);
		}
	}
	return result;
}

} // namespace Serein::Network
