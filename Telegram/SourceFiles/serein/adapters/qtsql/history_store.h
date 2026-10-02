#pragma once

#include "serein/ports/history_store.h"

#include <memory>

class QSqlQuery;

namespace Serein::Adapters {

class SqlHistoryStore final : public Ports::HistoryStore {
public:
	static constexpr auto kSchemaVersion = 1;

	[[nodiscard]] static std::unique_ptr<SqlHistoryStore> Open(
		const QString &path,
		Ports::Cipher &cipher,
		QString *error = nullptr);
	~SqlHistoryStore() override;

	[[nodiscard]] bool save(const History::Record &record) override;
	[[nodiscard]] std::vector<History::Record> deleted(
		const Ports::HistoryQuery &query) override;
	[[nodiscard]] std::vector<History::Record> versions(
		qint64 peerId,
		qint64 messageId) override;
	[[nodiscard]] std::vector<qint64> peersWithDeleted(int limit) override;
	[[nodiscard]] int nextRevision(qint64 peerId, qint64 messageId) override;
	[[nodiscard]] bool clearPeer(qint64 peerId) override;
	[[nodiscard]] bool clearAll() override;
	[[nodiscard]] bool prune(qint64 recordedBefore, int keepAtMost) override;
	void beginBatch() override;
	void endBatch() override;

	[[nodiscard]] int skippedRows() const;

private:
	SqlHistoryStore(QString connection, Ports::Cipher &cipher);
	void tune();

	[[nodiscard]] QString migrate();
	[[nodiscard]] std::vector<History::Record> collect(QSqlQuery &query);

	QString _connection;
	Ports::Cipher &_cipher;
	int _skippedRows = 0;
	int _batchDepth = 0;
	bool _batchOpen = false;

};

} // namespace Serein::Adapters
