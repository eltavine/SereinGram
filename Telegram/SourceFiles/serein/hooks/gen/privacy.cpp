// Generated from proto/serein/settings/v1/privacy.proto by tools/serein/codegen; do not edit.
#include "serein/hooks/gen/privacy.h"

#include "serein/core/options.h"
#include "serein/schema/gen/settings/privacy.h"

namespace Serein::Hooks::Privacy {

bool DemoMode() {
	return ForDevice().Get(Serein::Privacy::kDemoMode);
}

rpl::producer<bool> DemoModeValue() {
	return ForDevice().Value(Serein::Privacy::kDemoMode);
}

bool AutoDemoMode() {
	return ForDevice().Get(Serein::Privacy::kAutoDemoMode);
}

rpl::producer<bool> AutoDemoModeValue() {
	return ForDevice().Value(Serein::Privacy::kAutoDemoMode);
}

bool LockSettings() {
	return ForDevice().Get(Serein::Privacy::kLockSettings);
}

rpl::producer<bool> LockSettingsValue() {
	return ForDevice().Value(Serein::Privacy::kLockSettings);
}

bool HideReadTime() {
	return ForDevice().Get(Serein::Privacy::kHideReadTime);
}

rpl::producer<bool> HideReadTimeValue() {
	return ForDevice().Value(Serein::Privacy::kHideReadTime);
}

bool HideSharePhonePrompt() {
	return ForDevice().Get(Serein::Privacy::kHideSharePhonePrompt);
}

rpl::producer<bool> HideSharePhonePromptValue() {
	return ForDevice().Value(Serein::Privacy::kHideSharePhonePrompt);
}

int ProfileIdFormat() {
	return ForDevice().Get(Serein::Privacy::kProfileIdFormat);
}

rpl::producer<int> ProfileIdFormatValue() {
	return ForDevice().Value(Serein::Privacy::kProfileIdFormat);
}

bool ShowProfileDc() {
	return ForDevice().Get(Serein::Privacy::kShowProfileDc);
}

rpl::producer<bool> ShowProfileDcValue() {
	return ForDevice().Value(Serein::Privacy::kShowProfileDc);
}

bool ShowRegistrationDate() {
	return ForDevice().Get(Serein::Privacy::kShowRegistrationDate);
}

rpl::producer<bool> ShowRegistrationDateValue() {
	return ForDevice().Value(Serein::Privacy::kShowRegistrationDate);
}

bool ShowContactStatus() {
	return ForDevice().Get(Serein::Privacy::kShowContactStatus);
}

rpl::producer<bool> ShowContactStatusValue() {
	return ForDevice().Value(Serein::Privacy::kShowContactStatus);
}

bool ShowSessionDetails() {
	return ForDevice().Get(Serein::Privacy::kShowSessionDetails);
}

rpl::producer<bool> ShowSessionDetailsValue() {
	return ForDevice().Value(Serein::Privacy::kShowSessionDetails);
}

bool LocalNames() {
	return ForDevice().Get(Serein::Privacy::kLocalNames);
}

rpl::producer<bool> LocalNamesValue() {
	return ForDevice().Value(Serein::Privacy::kLocalNames);
}

bool HideProfileGifts() {
	return ForDevice().Get(Serein::Privacy::kHideProfileGifts);
}

rpl::producer<bool> HideProfileGiftsValue() {
	return ForDevice().Value(Serein::Privacy::kHideProfileGifts);
}

bool HideCreateTodo() {
	return ForDevice().Get(Serein::Privacy::kHideCreateTodo);
}

rpl::producer<bool> HideCreateTodoValue() {
	return ForDevice().Value(Serein::Privacy::kHideCreateTodo);
}

bool SaveProtectedContent() {
	return ForDevice().Get(Serein::Privacy::kSaveProtectedContent);
}

rpl::producer<bool> SaveProtectedContentValue() {
	return ForDevice().Value(Serein::Privacy::kSaveProtectedContent);
}

} // namespace Serein::Hooks::Privacy
