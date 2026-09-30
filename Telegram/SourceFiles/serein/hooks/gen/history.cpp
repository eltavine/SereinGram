// Generated from proto/serein/settings/v1/history.proto by tools/serein/codegen; do not edit.
#include "serein/hooks/gen/history.h"

#include "serein/core/options.h"
#include "serein/schema/gen/settings/history.h"

namespace Serein::Hooks::HistorySettings {

bool HistorySaveDeleted(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Get(Serein::HistorySettings::kHistorySaveDeleted);
}

rpl::producer<bool> HistorySaveDeletedValue(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Value(Serein::HistorySettings::kHistorySaveDeleted);
}

bool HistoryKeepDeletedInPlace(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Get(Serein::HistorySettings::kHistoryKeepDeletedInPlace);
}

rpl::producer<bool> HistoryKeepDeletedInPlaceValue(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Value(Serein::HistorySettings::kHistoryKeepDeletedInPlace);
}

bool HistorySaveEdits(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Get(Serein::HistorySettings::kHistorySaveEdits);
}

rpl::producer<bool> HistorySaveEditsValue(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Value(Serein::HistorySettings::kHistorySaveEdits);
}

bool HistoryIncludeBots(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Get(Serein::HistorySettings::kHistoryIncludeBots);
}

rpl::producer<bool> HistoryIncludeBotsValue(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Value(Serein::HistorySettings::kHistoryIncludeBots);
}

int HistoryRetentionDays(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Get(Serein::HistorySettings::kHistoryRetentionDays);
}

rpl::producer<int> HistoryRetentionDaysValue(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Value(Serein::HistorySettings::kHistoryRetentionDays);
}

int HistoryMaxRecords(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Get(Serein::HistorySettings::kHistoryMaxRecords);
}

rpl::producer<int> HistoryMaxRecordsValue(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Value(Serein::HistorySettings::kHistoryMaxRecords);
}

QByteArray HistoryExcludedPeers(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Get(Serein::HistorySettings::kHistoryExcludedPeers);
}

rpl::producer<QByteArray> HistoryExcludedPeersValue(gsl::not_null<Main::Session*> session) {
	return ForAccount(session).Value(Serein::HistorySettings::kHistoryExcludedPeers);
}

} // namespace Serein::Hooks::HistorySettings
