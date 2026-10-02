#pragma once

#include <QtCore/QRect>
#include <QtCore/QSize>
#include <QtCore/QString>

#include <cstdint>
#include <vector>

namespace Serein::Stories {

inline constexpr auto kDefaultPeriod = 86400;
inline constexpr auto kMaxVideoSeconds = 60;
inline constexpr auto kCanvasSize = QSize(1080, 1920);

enum class Audience {
	Everyone,
	Contacts,
	CloseFriends,
	Selected,
};

struct AudienceRule {
	enum class Kind {
		AllowAll,
		AllowContacts,
		AllowCloseFriends,
		AllowUsers,
		DisallowUsers,
	};

	Kind kind = Kind::AllowAll;
	std::vector<std::uint64_t> users;

	friend bool operator==(
		const AudienceRule &,
		const AudienceRule &) = default;
};

[[nodiscard]] bool TakesExclusions(Audience audience);
[[nodiscard]] bool TakesSelection(Audience audience);

// Empty when the audience cannot be posted to, like nobody selected.
[[nodiscard]] std::vector<AudienceRule> AudienceRules(
	Audience audience,
	const std::vector<std::uint64_t> &users);

[[nodiscard]] std::vector<int> PeriodChoices(bool premium);
[[nodiscard]] int EffectivePeriod(int seconds, bool premium);

[[nodiscard]] bool FillsCanvas(QSize content, QSize canvas);
[[nodiscard]] QRect FitRect(QSize content, QSize canvas);
[[nodiscard]] QRect CoverRect(QSize content, QSize canvas);

enum class PostError {
	TooMany,
	PremiumRequired,
	BoostsRequired,
	WeeklyLimit,
	MonthlyLimit,
	Other,
};

[[nodiscard]] PostError ClassifyError(const QString &type);

} // namespace Serein::Stories
