#include "serein/core/patterns.h"

#include "base/basic_types.h"

#include <map>
#include <utility>

namespace Serein {
namespace {

constexpr auto kMaxCachedPatterns = 256;

[[nodiscard]] QString Limits() {
	return u"(*NO_JIT)(*LIMIT_MATCH=10000)(*LIMIT_DEPTH=64)(*LIMIT_HEAP=1024)"_q;
}

} // namespace

QRegularExpression SafePattern(const QString &pattern, bool caseInsensitive) {
	return QRegularExpression(
		Limits() + pattern,
		QRegularExpression::UseUnicodePropertiesOption
			| (caseInsensitive
				? QRegularExpression::CaseInsensitiveOption
				: QRegularExpression::NoPatternOption));
}

QRegularExpression CachedSafePattern(
		const QString &pattern,
		bool caseInsensitive) {
	static auto cache = std::map<std::pair<QString, bool>, QRegularExpression>();
	auto key = std::pair(pattern, caseInsensitive);
	if (const auto i = cache.find(key); i != end(cache)) {
		return i->second;
	}
	if (cache.size() >= kMaxCachedPatterns) {
		cache.clear();
	}
	auto expression = SafePattern(pattern, caseInsensitive);
	expression.optimize();
	return cache.emplace(std::move(key), std::move(expression)).first->second;
}

} // namespace Serein
