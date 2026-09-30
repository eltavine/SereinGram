#include "serein/features/ghost/model/policy.h"

namespace Serein::Ghost {

Policy Read(Options &account) {
	return {
		.enabled = account.Get(kGhostMode),
		.hideReadReceipts = account.Get(kGhostHideReadReceipts),
		.hideStoryViews = account.Get(kGhostHideStoryViews),
		.hideOnline = account.Get(kGhostHideOnline),
		.hideTyping = account.Get(kGhostHideTyping),
		.hideViewIncrements = account.Get(kGhostHideViewIncrements),
		.markReadAfterSending = account.Get(kGhostMarkReadAfterSending),
		.useScheduledMessages = account.Get(kGhostUseScheduledMessages),
	};
}

bool Allows(const Policy &policy, Activity activity) {
	if (!policy.enabled) {
		return true;
	}
	switch (activity) {
	case Activity::ReadReceipt: return !policy.hideReadReceipts;
	case Activity::StoryView: return !policy.hideStoryViews;
	case Activity::Online: return !policy.hideOnline;
	case Activity::Typing: return !policy.hideTyping;
	case Activity::ViewIncrement: return !policy.hideViewIncrements;
	}
	return true;
}

bool OfflineAfterSending(const Policy &policy) {
	return policy.enabled && policy.hideOnline;
}

bool MarkReadAfterSending(const Policy &policy) {
	return policy.enabled && policy.markReadAfterSending;
}

bool ScheduleOutgoing(const Policy &policy) {
	return policy.enabled && policy.useScheduledMessages;
}

} // namespace Serein::Ghost
