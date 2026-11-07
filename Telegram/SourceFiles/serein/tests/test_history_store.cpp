#include "serein/adapters/qtsql/history_store.h"
#include "base/basic_types.h"
#include "serein/tests/require.h"

#include <doctest/doctest.h>
#include <QtCore/QTemporaryDir>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

#include <iostream>

namespace {

class ReversingCipher final : public Serein::Ports::Cipher {
public:
	QByteArray encrypt(const QByteArray &plain) override {
		auto result = plain;
		std::reverse(result.begin(), result.end());
		return "sealed:" + result;
	}
	std::optional<QByteArray> decrypt(const QByteArray &sealed) override {
		if (!sealed.startsWith("sealed:")) {
			return std::nullopt;
		}
		auto result = sealed.mid(7);
		std::reverse(result.begin(), result.end());
		return result;
	}

};

[[nodiscard]] Serein::History::Record Record(
		qint64 messageId,
		qint64 recordedAt,
		Serein::History::RecordKind kind = Serein::History::RecordKind::Deleted,
		int revision = 0) {
	auto result = Serein::History::Record();
	result.kind = kind;
	result.peerId = 777;
	result.messageId = messageId;
	result.revision = revision;
	result.recordedAt = recordedAt;
	result.text = u"message %1"_q.arg(messageId);
	return result;
}

} // namespace

TEST_CASE("HistoryStore") {
	using namespace Serein;
	using Kind = History::RecordKind;
	auto directory = QTemporaryDir();
	Require(directory.isValid(), "temporary directory");
	const auto path = directory.filePath(u"history.sqlite3"_q);
	auto cipher = ReversingCipher();
	{
		auto store = Adapters::SqlHistoryStore::Open(path, cipher);
		Require(store != nullptr, "history store opens");
		Require(store->save(Record(1, 100)), "deleted record saves");
		Require(store->save(Record(2, 300)), "second deleted record saves");
		Require(store->save(Record(3, 200, Kind::Edited, 0)), "edit saves");
		Require(store->save(Record(3, 250, Kind::Edited, 1)), "second edit saves");
		Require(!store->save(History::Record()), "invalid records are refused");

		const auto deleted = store->deleted({ .peerId = 777 });
		Require(deleted.size() == 2 && deleted[0].messageId == 2,
			"deleted records are listed newest first");
		Require(store->deleted({ .peerId = 777, .recordedBefore = 300 }).size() == 1,
			"deleted records page by time");
		const auto ranged = store->deleted({
			.peerId = 777,
			.minMessageId = 2,
			.maxMessageId = 5,
		});
		Require(ranged.size() == 1 && ranged[0].messageId == 2,
			"deleted records filter by message id range");
		Require(store->deleted({ .peerId = 777, .maxMessageId = 1 }).size() == 1,
			"deleted records filter by upper message id");
		Require(store->versions(777, 3).size() == 2, "edit versions are listed");
		Require(store->nextRevision(777, 3) == 2, "revisions continue");
		Require(store->nextRevision(777, 9) == 0, "revisions start at zero");
		auto otherPeer = Record(4, 400);
		otherPeer.peerId = 888;
		auto editOnly = Record(6, 600, Kind::Edited, 0);
		editOnly.peerId = 999;
		Require(store->save(otherPeer) && store->save(editOnly),
			"records of other chats save");
		using Summary = Ports::PeerSummary;
		Require(store->peersWithDeleted(std::nullopt) == std::vector<Summary>{
				{ .peerId = 888, .count = 1, .lastRecordedAt = 400 },
				{ .peerId = 777, .count = 2, .lastRecordedAt = 300 },
			},
			"chats with deleted records not listed newest first with counts");
		Require(store->peersWithDeleted(1).size() == 1
			&& store->peersWithDeleted(1)[0].peerId == 888,
			"chats with deleted records not limited");
		Require(store->clearPeer(888) && store->clearPeer(999),
			"other chats clear");
	}
	{
		auto store = Adapters::SqlHistoryStore::Open(path, cipher);
		Require(store && store->deleted({ .peerId = 777 }).size() == 2,
			"records persist across reopening");
		Require(store->prune(150, 100), "pruning by age succeeds");
		Require(store->deleted({ .peerId = 777 }).size() == 1, "old records are pruned");
		Require(store->prune(0, 1), "pruning by count succeeds");
		Require(store->versions(777, 3).empty() && store->deleted({ .peerId = 777 }).size() == 1,
			"only the newest records are kept");
		Require(store->clearPeer(777) && store->deleted({ .peerId = 777 }).empty(),
			"clearing a chat removes its records");
	}
	{
		auto store = Adapters::SqlHistoryStore::Open(path, cipher);
		Require(store != nullptr, "history store reopens for batches");
		store->beginBatch();
		store->beginBatch();
		Require(store->save(Record(6, 600)) && store->save(Record(7, 700)),
			"batched records save");
		store->endBatch();
		Require(store->deleted({ .peerId = 777 }).size() == 2,
			"batched records are visible inside the batch");
		store->endBatch();
	}
	{
		auto store = Adapters::SqlHistoryStore::Open(path, cipher);
		Require(store && store->deleted({ .peerId = 777 }).size() == 2,
			"batched records are committed");
		Require(store->clearPeer(777), "batched records clear");
	}
	{
		auto other = ReversingCipher();
		auto store = Adapters::SqlHistoryStore::Open(path, other);
		Require(store->save(Record(5, 500)), "record saves before corruption");
		auto database = QSqlDatabase::addDatabase(u"QSQLITE"_q, u"serein-test-raw"_q);
		database.setDatabaseName(path);
		Require(database.open(), "raw connection opens");
		auto corrupt = QSqlQuery(database);
		Require(corrupt.exec(u"UPDATE records SET payload = X'00'"_q), "payload corrupts");
		Require(store->deleted({ .peerId = 777 }).empty() && store->skippedRows() == 1,
			"undecryptable rows are skipped and counted");
		Require(corrupt.exec(u"PRAGMA user_version = 99"_q), "schema version bumps");
		corrupt = QSqlQuery();
		database.close();
		database = QSqlDatabase();
		QSqlDatabase::removeDatabase(u"serein-test-raw"_q);
	}
	auto error = QString();
	Require(!Adapters::SqlHistoryStore::Open(path, cipher, &error) && !error.isEmpty(),
		"newer schema versions are refused");
	std::cout << "PASS: Serein history store" << std::endl;
}

TEST_CASE("HistoryStorePaging") {
	using namespace Serein;
	using Kind = History::RecordKind;
	using Order = Ports::RecordsOrder;
	auto directory = QTemporaryDir();
	Require(directory.isValid(), "temporary directory");
	auto cipher = ReversingCipher();
	auto store = Adapters::SqlHistoryStore::Open(
		directory.filePath(u"history.sqlite3"_q),
		cipher);
	Require(store != nullptr, "history store opens");
	for (auto id = 1; id <= 250; ++id) {
		Require(store->save(Record(id, 1000 + id)), "deleted record saves");
	}
	Require(store->save(Record(10, 2000, Kind::Edited, 1)), "edit saves");
	Require(store->save(Record(10, 2001, Kind::Edited, 2)), "second edit saves");

	const auto ids = [](const std::vector<History::Record> &records) {
		auto result = std::vector<qint64>();
		for (const auto &record : records) {
			result.push_back(record.messageId);
		}
		return result;
	};
	const auto deleted = Ports::RecordsQuery{ .peerId = 777, .kind = Kind::Deleted };
	Require(store->count(deleted) == 250,
		"every deleted record is counted beyond any former display limit");
	Require(store->records(deleted).size() == 250,
		"records without a limit are all returned");

	auto newest = deleted;
	newest.order = Order::Descending;
	newest.limit = 3;
	Require(ids(store->records(newest)) == std::vector<qint64>{ 250, 249, 248 },
		"newest records come first in descending order");

	auto older = deleted;
	older.till = Ports::RecordBound{ .key = { 100, 0 } };
	older.order = Order::Descending;
	older.limit = 2;
	Require(ids(store->records(older)) == std::vector<qint64>{ 99, 98 },
		"records strictly before a key page backwards");
	Require(store->count(older) == 99, "records before a key are counted");

	auto newer = deleted;
	newer.from = Ports::RecordBound{ .key = { 100, 0 }, .inclusive = true };
	newer.limit = 2;
	Require(ids(store->records(newer)) == std::vector<qint64>{ 100, 101 },
		"inclusive lower bound keeps the key itself");
	newer.from->inclusive = false;
	Require(ids(store->records(newer)) == std::vector<qint64>{ 101, 102 },
		"exclusive lower bound skips the key");

	auto versions = Ports::RecordsQuery{ .peerId = 777, .messageId = 10 };
	const auto all = store->records(versions);
	Require(all.size() == 3
		&& all[0].kind == Kind::Deleted
		&& all[1].revision == 1
		&& all[2].revision == 2,
		"every version of one message is listed by revision");
	versions.kind = Kind::Edited;
	Require(store->count(versions) == 2, "versions filter by kind");
	auto otherPeer = deleted;
	otherPeer.peerId = 888;
	Require(store->count(otherPeer) == 0 && store->records(otherPeer).empty(),
		"other chats are not mixed in");
	Require(store->deleted({ .peerId = 777 }).size() == 250,
		"the legacy listing is no longer capped by default");
	std::cout << "PASS: Serein history store paging" << std::endl;
}
