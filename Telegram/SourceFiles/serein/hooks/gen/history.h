// Generated from proto/serein/settings/v1/history.proto by tools/serein/codegen; do not edit.
#pragma once

#include <gsl/pointers>
#include <rpl/producer.h>

namespace Main {
class Session;
} // namespace Main

namespace Serein::Hooks::HistorySettings {

[[nodiscard]] bool HistorySaveDeleted(gsl::not_null<Main::Session*> session);
[[nodiscard]] rpl::producer<bool> HistorySaveDeletedValue(gsl::not_null<Main::Session*> session);
[[nodiscard]] bool HistorySaveEdits(gsl::not_null<Main::Session*> session);
[[nodiscard]] rpl::producer<bool> HistorySaveEditsValue(gsl::not_null<Main::Session*> session);
[[nodiscard]] bool HistoryIncludeBots(gsl::not_null<Main::Session*> session);
[[nodiscard]] rpl::producer<bool> HistoryIncludeBotsValue(gsl::not_null<Main::Session*> session);
[[nodiscard]] int HistoryRetentionDays(gsl::not_null<Main::Session*> session);
[[nodiscard]] rpl::producer<int> HistoryRetentionDaysValue(gsl::not_null<Main::Session*> session);
[[nodiscard]] int HistoryMaxRecords(gsl::not_null<Main::Session*> session);
[[nodiscard]] rpl::producer<int> HistoryMaxRecordsValue(gsl::not_null<Main::Session*> session);

} // namespace Serein::Hooks::HistorySettings
