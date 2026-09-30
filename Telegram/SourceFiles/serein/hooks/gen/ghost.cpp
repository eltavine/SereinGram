// Generated from proto/serein/settings/v1/ghost.proto by tools/serein/codegen; do not edit.
#include "serein/hooks/gen/ghost.h"

#include "serein/core/options.h"
#include "serein/schema/gen/settings/ghost.h"

namespace Serein::Hooks::Ghost {

bool GhostMode(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Get(Serein::Ghost::kGhostMode);
}

rpl::producer<bool> GhostModeValue(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Value(Serein::Ghost::kGhostMode);
}

bool GhostHideReadReceipts(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Get(Serein::Ghost::kGhostHideReadReceipts);
}

rpl::producer<bool> GhostHideReadReceiptsValue(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Value(Serein::Ghost::kGhostHideReadReceipts);
}

bool GhostHideStoryViews(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Get(Serein::Ghost::kGhostHideStoryViews);
}

rpl::producer<bool> GhostHideStoryViewsValue(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Value(Serein::Ghost::kGhostHideStoryViews);
}

bool GhostHideOnline(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Get(Serein::Ghost::kGhostHideOnline);
}

rpl::producer<bool> GhostHideOnlineValue(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Value(Serein::Ghost::kGhostHideOnline);
}

bool GhostHideTyping(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Get(Serein::Ghost::kGhostHideTyping);
}

rpl::producer<bool> GhostHideTypingValue(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Value(Serein::Ghost::kGhostHideTyping);
}

bool GhostHideViewIncrements(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Get(Serein::Ghost::kGhostHideViewIncrements);
}

rpl::producer<bool> GhostHideViewIncrementsValue(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Value(Serein::Ghost::kGhostHideViewIncrements);
}

bool GhostMarkReadAfterSending(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Get(Serein::Ghost::kGhostMarkReadAfterSending);
}

rpl::producer<bool> GhostMarkReadAfterSendingValue(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Value(Serein::Ghost::kGhostMarkReadAfterSending);
}

bool GhostUseScheduledMessages(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Get(Serein::Ghost::kGhostUseScheduledMessages);
}

rpl::producer<bool> GhostUseScheduledMessagesValue(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Value(Serein::Ghost::kGhostUseScheduledMessages);
}

} // namespace Serein::Hooks::Ghost
