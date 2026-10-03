#pragma once

#include "base/basic_types.h"

#include <optional>

namespace Serein::Cleanup {

enum class Category {
	DeletedAccount,
	UnusedBot,
	InactiveChat,
};

struct ChatFacts {
	bool user = false;
	bool deleted = false;
	bool bot = false;
	bool kept = false;
	TimeId lastActivity = 0;
};

inline constexpr auto kInactiveDays = 180;

[[nodiscard]] std::optional<Category> Classify(
	const ChatFacts &facts,
	TimeId now,
	int inactiveDays = kInactiveDays);

} // namespace Serein::Cleanup
