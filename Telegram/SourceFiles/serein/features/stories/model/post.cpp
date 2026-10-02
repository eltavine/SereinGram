#include "serein/features/stories/model/post.h"

#include "base/basic_types.h"

#include <algorithm>
#include <cmath>

namespace Serein::Stories {
namespace {

constexpr auto kHour = 3600;
constexpr auto kAspectTolerance = 0.01;

[[nodiscard]] QRect Centered(QSize size, QSize canvas) {
	return QRect(
		QPoint(
			(canvas.width() - size.width()) / 2,
			(canvas.height() - size.height()) / 2),
		size);
}

[[nodiscard]] QSize Scaled(QSize content, double scale) {
	return QSize(
		std::max(1, int(std::lround(content.width() * scale))),
		std::max(1, int(std::lround(content.height() * scale))));
}

} // namespace

bool TakesExclusions(Audience audience) {
	return (audience == Audience::Everyone)
		|| (audience == Audience::Contacts);
}

bool TakesSelection(Audience audience) {
	return (audience == Audience::Selected);
}

std::vector<AudienceRule> AudienceRules(
		Audience audience,
		const std::vector<std::uint64_t> &users) {
	using Kind = AudienceRule::Kind;
	auto result = std::vector<AudienceRule>();
	const auto exclude = [&] {
		if (!users.empty()) {
			result.push_back({ Kind::DisallowUsers, users });
		}
	};
	switch (audience) {
	case Audience::Everyone:
		exclude();
		result.push_back({ Kind::AllowAll });
		break;
	case Audience::Contacts:
		exclude();
		result.push_back({ Kind::AllowContacts });
		break;
	case Audience::CloseFriends:
		result.push_back({ Kind::AllowCloseFriends });
		break;
	case Audience::Selected:
		if (!users.empty()) {
			result.push_back({ Kind::AllowUsers, users });
		}
		break;
	}
	return result;
}

ParsedAudience ParseAudience(const std::vector<AudienceRule> &rules) {
	using Kind = AudienceRule::Kind;
	const auto has = [&](Kind kind) {
		return std::any_of(rules.begin(), rules.end(), [&](
				const AudienceRule &rule) {
			return rule.kind == kind;
		});
	};
	const auto users = [&](Kind kind) {
		auto result = std::vector<std::uint64_t>();
		for (const auto &rule : rules) {
			if (rule.kind == kind) {
				result.insert(end(result), begin(rule.users), end(rule.users));
			}
		}
		return result;
	};
	if (has(Kind::AllowCloseFriends)) {
		return { Audience::CloseFriends };
	} else if (has(Kind::AllowAll)) {
		return { Audience::Everyone, users(Kind::DisallowUsers) };
	} else if (has(Kind::AllowContacts)) {
		return { Audience::Contacts, users(Kind::DisallowUsers) };
	}
	return { Audience::Selected, users(Kind::AllowUsers) };
}

std::vector<int> PeriodChoices(bool premium) {
	if (!premium) {
		return { kDefaultPeriod };
	}
	return { 6 * kHour, 12 * kHour, kDefaultPeriod, 2 * kDefaultPeriod };
}

int EffectivePeriod(int seconds, bool premium) {
	const auto choices = PeriodChoices(premium);
	return (std::find(choices.begin(), choices.end(), seconds)
		!= choices.end())
		? seconds
		: kDefaultPeriod;
}

bool FillsCanvas(QSize content, QSize canvas) {
	if (content.isEmpty() || canvas.isEmpty()) {
		return false;
	}
	const auto wanted = double(canvas.width()) / canvas.height();
	const auto actual = double(content.width()) / content.height();
	return std::abs(actual - wanted) <= wanted * kAspectTolerance;
}

QRect FitRect(QSize content, QSize canvas) {
	if (content.isEmpty() || canvas.isEmpty()) {
		return QRect(QPoint(), canvas);
	}
	const auto scale = std::min(
		double(canvas.width()) / content.width(),
		double(canvas.height()) / content.height());
	return Centered(Scaled(content, scale), canvas);
}

QRect CoverRect(QSize content, QSize canvas) {
	if (content.isEmpty() || canvas.isEmpty()) {
		return QRect(QPoint(), canvas);
	}
	const auto scale = std::max(
		double(canvas.width()) / content.width(),
		double(canvas.height()) / content.height());
	return Centered(Scaled(content, scale), canvas);
}

PostError ClassifyError(const QString &type) {
	if (type == u"STORIES_TOO_MUCH"_q) {
		return PostError::TooMany;
	} else if (type == u"PREMIUM_ACCOUNT_REQUIRED"_q) {
		return PostError::PremiumRequired;
	} else if (type == u"BOOSTS_REQUIRED"_q) {
		return PostError::BoostsRequired;
	} else if (type.startsWith(u"STORY_SEND_FLOOD_WEEKLY_"_q)) {
		return PostError::WeeklyLimit;
	} else if (type.startsWith(u"STORY_SEND_FLOOD_MONTHLY_"_q)) {
		return PostError::MonthlyLimit;
	}
	return PostError::Other;
}

} // namespace Serein::Stories
