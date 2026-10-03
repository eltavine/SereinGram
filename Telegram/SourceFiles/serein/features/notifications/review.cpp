#include "serein/hooks/interface/notifications.h"

#include "serein/core/options.h"
#include "serein/features/keyword_alerts/model/keywords.h"
#include "serein/features/quiet_hours/model/schedule.h"
#include "serein/schema/gen/settings/filters.h"
#include "serein/schema/gen/settings/interface.h"
#include "data/data_peer.h"
#include "data/data_user.h"
#include "history/history.h"
#include "history/history_item.h"

#include <QtCore/QDateTime>

namespace Serein::Interface {
namespace {

template <typename Document, typename Read>
[[nodiscard]] const Document &Cached(const QByteArray &raw, Read read) {
	static auto last = QByteArray();
	static auto parsed = Document();
	if (raw != last) {
		parsed = read(raw).value_or(Document());
		last = raw;
	}
	return parsed;
}

[[nodiscard]] bool KeywordMatch(not_null<HistoryItem*> item, bool message) {
	if (!message || item->out()) {
		return false;
	}
	const auto history = item->history();
	const auto raw = ForAccount(&history->session()).Get(
		Filters::kKeywordAlerts);
	const auto &config = Cached<Notifications::KeywordAlerts>(
		raw,
		Notifications::ReadKeywordAlerts);
	return Notifications::Matches(
		config,
		item->originalText().text,
		history->peer->isBroadcast());
}

[[nodiscard]] bool Quiet(
		not_null<HistoryItem*> item,
		bool message,
		bool keywordMatch) {
	const auto raw = ForDevice().Get(kQuietHours);
	const auto &config = Cached<Notifications::QuietHours>(
		raw,
		Notifications::ReadQuietHours);
	if (!config.enabled) {
		return false;
	}
	const auto from = item->from()->asUser();
	const auto now = QDateTime::currentDateTime();
	return Notifications::Silences(config, {
		.message = message,
		.fromContact = from && from->isContact(),
		.pinnedChat = item->history()->isPinnedDialog(0),
		.mentionsMe = message && item->mentionsMe(),
		.keywordMatch = keywordMatch,
	}, now.date().dayOfWeek(), now.time().hour() * 60 + now.time().minute());
}

} // namespace

std::optional<bool> ReviewNotification(
		not_null<HistoryItem*> item,
		bool message) {
	const auto keywordMatch = KeywordMatch(item, message);
	if (Quiet(item, message, keywordMatch)) {
		return false;
	} else if (keywordMatch) {
		return true;
	}
	return std::nullopt;
}

} // namespace Serein::Interface
