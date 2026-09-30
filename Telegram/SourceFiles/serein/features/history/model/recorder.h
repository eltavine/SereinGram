#pragma once

#include "serein/ports/history_store.h"
#include "serein/schema/gen/config/history_exclusions.h"
#include "serein/schema/gen/settings/history.h"

#include <functional>
#include <set>

namespace Serein::HistoryFeature {

struct Snapshot {
	qint64 peerId = 0;
	qint64 messageId = 0;
	qint64 topicRootId = 0;
	qint64 date = 0;
	qint64 fromPeerId = 0;
	bool fromBot = false;
	QString text;
	std::vector<History::TextEntity> entities;
	QString mediaSummary;
	QString localPath;
	QString cachedMediaName;
};

struct Policy {
	bool saveDeleted = false;
	bool saveEdits = false;
	bool includeBots = false;
	int retentionDays = 0;
	int maxRecords = 0;
	std::set<qint64> excludedPeers;
};

[[nodiscard]] Policy Read(Options &account);
[[nodiscard]] bool Excluded(Options &account, qint64 peerId);
[[nodiscard]] bool SetExcluded(Options &account, qint64 peerId, bool excluded);

class Recorder final {
public:
	Recorder(Ports::HistoryStore &store, std::function<qint64()> now);

	bool recordDeleted(const Policy &policy, const Snapshot &snapshot);
	bool recordEdit(const Policy &policy, const Snapshot &before);
	bool prune(const Policy &policy);

private:
	[[nodiscard]] bool record(History::RecordKind kind, const Snapshot &snapshot);

	Ports::HistoryStore &_store;
	std::function<qint64()> _now;
};

} // namespace Serein::HistoryFeature
