#pragma once

#include "serein/schema/gen/settings/ghost.h"

namespace Serein::Ghost {

enum class Activity {
	ReadReceipt,
	StoryView,
	Online,
	Typing,
	ViewIncrement,
};

struct Policy {
	bool enabled = false;
	bool hideReadReceipts = true;
	bool hideStoryViews = true;
	bool hideOnline = true;
	bool hideTyping = true;
	bool hideViewIncrements = false;
	bool markReadAfterSending = false;
	bool useScheduledMessages = false;
};

[[nodiscard]] Policy Read(Options &account);
[[nodiscard]] bool Allows(const Policy &policy, Activity activity);
[[nodiscard]] bool OfflineAfterSending(const Policy &policy);
[[nodiscard]] bool MarkReadAfterSending(const Policy &policy);
[[nodiscard]] bool ScheduleOutgoing(const Policy &policy);

} // namespace Serein::Ghost
