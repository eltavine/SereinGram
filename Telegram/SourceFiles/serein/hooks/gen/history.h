// Generated from proto/serein/settings/v1/history.proto by tools/serein/codegen; do not edit.
#pragma once

#include <QtCore/QByteArray>
#include <gsl/pointers>
#include <rpl/producer.h>

namespace Main {
class Session;
} // namespace Main

namespace Serein::Hooks::HistorySettings {

[[nodiscard]] bool HistorySaveDeleted(gsl::not_null<Main::Session*> session);
[[nodiscard]] rpl::producer<bool> HistorySaveDeletedValue(gsl::not_null<Main::Session*> session);
[[nodiscard]] bool HistoryKeepDeletedInPlace(gsl::not_null<Main::Session*> session);
[[nodiscard]] rpl::producer<bool> HistoryKeepDeletedInPlaceValue(gsl::not_null<Main::Session*> session);
[[nodiscard]] bool HistoryKeepExpiredMedia(gsl::not_null<Main::Session*> session);
[[nodiscard]] rpl::producer<bool> HistoryKeepExpiredMediaValue(gsl::not_null<Main::Session*> session);
[[nodiscard]] bool HistoryKeepRemovedChats(gsl::not_null<Main::Session*> session);
[[nodiscard]] rpl::producer<bool> HistoryKeepRemovedChatsValue(gsl::not_null<Main::Session*> session);
[[nodiscard]] bool HistorySaveEdits(gsl::not_null<Main::Session*> session);
[[nodiscard]] rpl::producer<bool> HistorySaveEditsValue(gsl::not_null<Main::Session*> session);
[[nodiscard]] bool HistoryIncludeBots(gsl::not_null<Main::Session*> session);
[[nodiscard]] rpl::producer<bool> HistoryIncludeBotsValue(gsl::not_null<Main::Session*> session);
[[nodiscard]] int HistoryRetentionDays(gsl::not_null<Main::Session*> session);
[[nodiscard]] rpl::producer<int> HistoryRetentionDaysValue(gsl::not_null<Main::Session*> session);
[[nodiscard]] int HistoryMaxRecords(gsl::not_null<Main::Session*> session);
[[nodiscard]] rpl::producer<int> HistoryMaxRecordsValue(gsl::not_null<Main::Session*> session);
[[nodiscard]] QByteArray HistoryExcludedPeers(gsl::not_null<Main::Session*> session);
[[nodiscard]] rpl::producer<QByteArray> HistoryExcludedPeersValue(gsl::not_null<Main::Session*> session);

} // namespace Serein::Hooks::HistorySettings
