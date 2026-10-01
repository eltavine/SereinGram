// Generated from proto/serein/settings/v1/ghost.proto by tools/serein/codegen; do not edit.
#pragma once

#include <QtCore/QString>
#include <gsl/pointers>
#include <rpl/producer.h>

namespace Main {
class Session;
} // namespace Main

namespace Serein::Hooks::Ghost {

[[nodiscard]] bool GhostMode(gsl::not_null<Main::Session*> session);
[[nodiscard]] rpl::producer<bool> GhostModeValue(gsl::not_null<Main::Session*> session);
[[nodiscard]] bool GhostAllAccounts();
[[nodiscard]] rpl::producer<bool> GhostAllAccountsValue();
[[nodiscard]] bool GhostHideReadReceipts(gsl::not_null<Main::Session*> session);
[[nodiscard]] rpl::producer<bool> GhostHideReadReceiptsValue(gsl::not_null<Main::Session*> session);
[[nodiscard]] bool GhostHideStoryViews(gsl::not_null<Main::Session*> session);
[[nodiscard]] rpl::producer<bool> GhostHideStoryViewsValue(gsl::not_null<Main::Session*> session);
[[nodiscard]] bool GhostHideOnline(gsl::not_null<Main::Session*> session);
[[nodiscard]] rpl::producer<bool> GhostHideOnlineValue(gsl::not_null<Main::Session*> session);
[[nodiscard]] bool GhostHideTyping(gsl::not_null<Main::Session*> session);
[[nodiscard]] rpl::producer<bool> GhostHideTypingValue(gsl::not_null<Main::Session*> session);
[[nodiscard]] bool GhostHideViewIncrements(gsl::not_null<Main::Session*> session);
[[nodiscard]] rpl::producer<bool> GhostHideViewIncrementsValue(gsl::not_null<Main::Session*> session);
[[nodiscard]] bool GhostMarkReadAfterSending(gsl::not_null<Main::Session*> session);
[[nodiscard]] rpl::producer<bool> GhostMarkReadAfterSendingValue(gsl::not_null<Main::Session*> session);
[[nodiscard]] bool GhostExplicitReadReceipts(gsl::not_null<Main::Session*> session);
[[nodiscard]] rpl::producer<bool> GhostExplicitReadReceiptsValue(gsl::not_null<Main::Session*> session);
[[nodiscard]] bool GhostSendSilently(gsl::not_null<Main::Session*> session);
[[nodiscard]] rpl::producer<bool> GhostSendSilentlyValue(gsl::not_null<Main::Session*> session);
[[nodiscard]] bool GhostUseScheduledMessages(gsl::not_null<Main::Session*> session);
[[nodiscard]] rpl::producer<bool> GhostUseScheduledMessagesValue(gsl::not_null<Main::Session*> session);
[[nodiscard]] QString ReadReceiptExceptions(gsl::not_null<Main::Session*> session);
[[nodiscard]] rpl::producer<QString> ReadReceiptExceptionsValue(gsl::not_null<Main::Session*> session);

} // namespace Serein::Hooks::Ghost
