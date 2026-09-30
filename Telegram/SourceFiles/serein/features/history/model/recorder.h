#pragma once

#include "serein/ports/history_store.h"
#include "serein/schema/gen/settings/history.h"

#include <functional>

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
};

struct Policy {
	bool saveDeleted = false;
	bool saveEdits = false;
	bool includeBots = false;
	int retentionDays = 0;
	int maxRecords = 0;
};

[[nodiscard]] Policy Read(Options &account);

class Recorder final {
public:
	Recorder(Ports::HistoryStore &store, std::function<qint64()> now);

	[[nodiscard]] bool recordDeleted(const Policy &policy, const Snapshot &snapshot);
	[[nodiscard]] bool recordEdit(const Policy &policy, const Snapshot &before);
	[[nodiscard]] bool prune(const Policy &policy);

private:
	[[nodiscard]] bool record(History::RecordKind kind, const Snapshot &snapshot);

	Ports::HistoryStore &_store;
	std::function<qint64()> _now;
};

} // namespace Serein::HistoryFeature
