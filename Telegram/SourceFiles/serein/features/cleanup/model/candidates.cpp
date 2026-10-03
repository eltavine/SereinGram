#include "serein/features/cleanup/model/candidates.h"

namespace Serein::Cleanup {
namespace {

constexpr auto kSecondsPerDay = 86400;

} // namespace

std::optional<Category> Classify(
		const ChatFacts &facts,
		TimeId now,
		int inactiveDays) {
	if (facts.kept) {
		return std::nullopt;
	} else if (facts.user && facts.deleted) {
		return Category::DeletedAccount;
	}
	const auto inactive = (facts.lastActivity > 0)
		&& (int64(now) - facts.lastActivity
			>= int64(inactiveDays) * kSecondsPerDay);
	if (!inactive) {
		return std::nullopt;
	} else if (facts.user) {
		return facts.bot
			? std::make_optional(Category::UnusedBot)
			: std::nullopt;
	}
	return Category::InactiveChat;
}

} // namespace Serein::Cleanup
