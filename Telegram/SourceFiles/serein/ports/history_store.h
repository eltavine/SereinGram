#pragma once

#include "serein/schema/gen/history/record.h"

#include <optional>
#include <vector>

namespace Serein::Ports {

class Cipher {
public:
	virtual ~Cipher() = default;

	[[nodiscard]] virtual QByteArray encrypt(const QByteArray &plain) = 0;
	[[nodiscard]] virtual std::optional<QByteArray> decrypt(
		const QByteArray &sealed) = 0;

};

struct HistoryQuery {
	qint64 peerId = 0;
	std::optional<qint64> topicRootId;
	std::optional<qint64> recordedBefore;
	std::optional<qint64> minMessageId;
	std::optional<qint64> maxMessageId;
	int limit = 50;
};

class HistoryStore {
public:
	virtual ~HistoryStore() = default;

	[[nodiscard]] virtual bool save(const History::Record &record) = 0;
	[[nodiscard]] virtual std::vector<History::Record> deleted(
		const HistoryQuery &query) = 0;
	[[nodiscard]] virtual std::vector<History::Record> versions(
		qint64 peerId,
		qint64 messageId) = 0;
	[[nodiscard]] virtual std::vector<qint64> peersWithDeleted(int limit) = 0;
	[[nodiscard]] virtual int nextRevision(qint64 peerId, qint64 messageId) = 0;
	[[nodiscard]] virtual bool clearPeer(qint64 peerId) = 0;
	[[nodiscard]] virtual bool clearAll() = 0;
	[[nodiscard]] virtual bool prune(qint64 recordedBefore, int keepAtMost) = 0;
	virtual void beginBatch() {
	}
	virtual void endBatch() {
	}

};

} // namespace Serein::Ports
