#pragma once

#include "serein/schema/gen/history/record.h"

#include <compare>
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
	std::optional<int> limit;
};

struct RecordKey {
	qint64 messageId = 0;
	int revision = 0;

	friend inline auto operator<=>(
		const RecordKey &,
		const RecordKey &) = default;
};

struct RecordBound {
	RecordKey key;
	bool inclusive = false;
};

enum class RecordsOrder {
	Ascending,
	Descending,
};

struct RecordsQuery {
	qint64 peerId = 0;
	std::optional<History::RecordKind> kind;
	std::optional<qint64> messageId;
	std::optional<RecordBound> from;
	std::optional<RecordBound> till;
	RecordsOrder order = RecordsOrder::Ascending;
	std::optional<int> limit;
};

struct PeerSummary {
	qint64 peerId = 0;
	int count = 0;
	qint64 lastRecordedAt = 0;

	friend inline bool operator==(
		const PeerSummary &,
		const PeerSummary &) = default;
};

[[nodiscard]] inline RecordKey KeyOf(const History::Record &record) {
	return { record.messageId, record.revision };
}

class HistoryStore {
public:
	virtual ~HistoryStore() = default;

	[[nodiscard]] virtual bool save(const History::Record &record) = 0;
	[[nodiscard]] virtual std::vector<History::Record> deleted(
		const HistoryQuery &query) = 0;
	[[nodiscard]] virtual std::vector<History::Record> versions(
		qint64 peerId,
		qint64 messageId) = 0;
	[[nodiscard]] virtual std::vector<History::Record> records(
		const RecordsQuery &query) = 0;
	// Ignores the order and the limit of the query.
	[[nodiscard]] virtual int count(const RecordsQuery &query) = 0;
	[[nodiscard]] virtual std::vector<PeerSummary> peersWithDeleted(
		std::optional<int> limit) = 0;
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
