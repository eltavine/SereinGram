#pragma once

#include "serein/schema/gen/config/link_inline_bots.h"

#include <QtCore/QRegularExpression>

#include <optional>
#include <utility>
#include <vector>

namespace Serein::Compose {

inline constexpr auto kMaxInlineBotQuery = 256;

[[nodiscard]] std::optional<LinkInlineBots> ReadLinkInlineBots(
	const QByteArray &raw);
[[nodiscard]] QByteArray WriteLinkInlineBots(const LinkInlineBots &value);
[[nodiscard]] QString FormatLinkInlineBotLines(const LinkInlineBots &value);
[[nodiscard]] std::optional<LinkInlineBots> ParseLinkInlineBotLines(
	const QString &text);

class LinkInlineBotMatcher final {
public:
	explicit LinkInlineBotMatcher(const LinkInlineBots &rules);

	[[nodiscard]] QString match(const QString &text) const;

private:
	std::vector<std::pair<QString, QRegularExpression>> _rules;

};

} // namespace Serein::Compose
