#include "serein/privacy/alias_rules.h"

#include "serein/schema/gen/config/aliases.h"

#include <algorithm>

namespace Serein::Privacy {
namespace {

constexpr auto kMaximumAliasLength = 96;

} // namespace

bool ValidAlias(const QString &value) {
	return value.size() <= kMaximumAliasLength
		&& value == value.trimmed()
		&& QString::fromUtf8(value.toUtf8()) == value
		&& std::none_of(value.begin(), value.end(), [](QChar ch) {
			return ch.category() == QChar::Other_Control
				|| ch.category() == QChar::Separator_Line
				|| ch.category() == QChar::Separator_Paragraph;
		});
}

bool ValidPeerAliasesConfig(const PeerAliasesConfig &value) {
	return std::all_of(value.names.begin(), value.names.end(), [](
			const auto &entry) {
		auto ok = false;
		return entry.first.toULongLong(&ok) && ok && ValidAlias(entry.second);
	});
}

} // namespace Serein::Privacy
