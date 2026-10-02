#include "serein/hooks/compose/inline_bot.h"

#include "serein/compose/link_inline_bots.h"
#include "serein/compose/options.h"
#include "serein/core/options.h"

#include <optional>

namespace Serein::Compose {

QString InlineBotForLink(const QString &text) {
	struct Cache {
		QByteArray raw;
		std::optional<LinkInlineBotMatcher> matcher;
	};
	static auto cache = Cache();
	const auto raw = ForDevice().Get(kLinkInlineBots);
	if (raw.isEmpty()) {
		return QString();
	} else if (!cache.matcher || cache.raw != raw) {
		cache.raw = raw;
		cache.matcher.emplace(
			ReadLinkInlineBots(raw).value_or(LinkInlineBots()));
	}
	return cache.matcher->match(text);
}

} // namespace Serein::Compose
