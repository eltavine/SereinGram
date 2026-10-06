// Generated from proto/serein/settings/v1/history.proto by tools/serein/codegen; do not edit.
#pragma once

#include "serein/core/options.h"

namespace Serein::HistorySettings {

[[nodiscard]] bool ValidHistoryExclusions(const QByteArray &value);

inline constexpr auto kHistorySaveDeleted = Option<bool>{
	"serein.historySaveDeleted",
	Scope::Account,
	false,
	Category::Privacy,
	"lng_serein_history_save_deleted",
	0 };
inline constexpr auto kHistorySaveEdits = Option<bool>{
	"serein.historySaveEdits",
	Scope::Account,
	false,
	Category::Privacy,
	"lng_serein_history_save_edits",
	0 };
inline constexpr auto kHistoryIncludeBots = Option<bool>{
	"serein.historyIncludeBots",
	Scope::Account,
	false,
	Category::Privacy,
	"lng_serein_history_include_bots",
	0 };
inline constexpr auto kHistoryKeepDeletedInPlace = Option<bool>{
	"serein.historyKeepDeletedInPlace",
	Scope::Account,
	false,
	Category::Privacy,
	"lng_serein_history_keep_deleted_in_place",
	0 };
inline constexpr auto kHistoryQuoteDeletedReplies = Option<bool>{
	"serein.historyQuoteDeletedReplies",
	Scope::Account,
	true,
	Category::Privacy,
	"lng_serein_history_quote_deleted_replies",
	0 };
inline constexpr auto kHistoryKeepExpiredMedia = Option<bool>{
	"serein.historyKeepExpiredMedia",
	Scope::Account,
	false,
	Category::Privacy,
	"lng_serein_history_keep_expired_media",
	0 };
inline constexpr auto kHistoryKeepRemovedChats = Option<bool>{
	"serein.historyKeepRemovedChats",
	Scope::Account,
	false,
	Category::Privacy,
	"lng_serein_history_keep_removed_chats",
	0 };
inline constexpr auto kHistoryRetentionDays = Option<int>{
	"serein.historyRetentionDays",
	Scope::Account,
	0,
	Category::Privacy,
	"lng_serein_history_retention_days",
	0,
	[](const int &value) {
		return (value == 0)
			|| ((value >= 0) && (value <= 3650));
	} };
inline constexpr auto kHistoryMaxRecords = Option<int>{
	"serein.historyMaxRecords",
	Scope::Account,
	0,
	Category::Privacy,
	"lng_serein_history_max_records",
	0,
	[](const int &value) {
		return (value == 0)
			|| ((value >= 0) && (value <= 10000000));
	} };
inline const auto kHistoryExcludedPeers = Option<QByteArray>{
	"serein.historyExcludedPeers",
	Scope::Account,
	QByteArray(),
	Category::Privacy,
	"lng_serein_history_excluded_peers",
	static_cast<unsigned>(Flag::Hidden),
	&ValidHistoryExclusions };

inline void RegisterOptions(Registry &registry) {
	Expects(registry.Add(kHistorySaveDeleted));
	Expects(registry.Add(kHistorySaveEdits));
	Expects(registry.Add(kHistoryIncludeBots));
	Expects(registry.Add(kHistoryKeepDeletedInPlace));
	Expects(registry.Add(kHistoryQuoteDeletedReplies));
	Expects(registry.Add(kHistoryKeepExpiredMedia));
	Expects(registry.Add(kHistoryKeepRemovedChats));
	Expects(registry.Add(kHistoryRetentionDays));
	Expects(registry.Add(kHistoryMaxRecords));
	Expects(registry.Add(kHistoryExcludedPeers));
}

} // namespace Serein::HistorySettings
