#include "serein/adapters/qtsql/history_store.h"

#include "base/basic_types.h"

#include <QtCore/QUuid>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

namespace Serein::Adapters {
namespace {

[[nodiscard]] QSqlDatabase Database(const QString &connection) {
	return QSqlDatabase::database(connection, false);
}

[[nodiscard]] bool Run(QSqlQuery &query) {
	return query.exec();
}

[[nodiscard]] bool Run(const QString &connection, const QString &sql) {
	auto query = QSqlQuery(Database(connection));
	return query.exec(sql);
}

} // namespace

std::unique_ptr<SqlHistoryStore> SqlHistoryStore::Open(
		const QString &path,
		Ports::Cipher &cipher,
		QString *error) {
	const auto connection = u"serein-history-"_q
		+ QUuid::createUuid().toString(QUuid::WithoutBraces);
	{
		auto database = QSqlDatabase::addDatabase(u"QSQLITE"_q, connection);
		database.setDatabaseName(path);
		if (!database.open()) {
			if (error) {
				*error = database.lastError().text();
			}
			database = QSqlDatabase();
			QSqlDatabase::removeDatabase(connection);
			return nullptr;
		}
	}
	auto result = std::unique_ptr<SqlHistoryStore>(
		new SqlHistoryStore(connection, cipher));
	if (const auto failure = result->migrate(); !failure.isEmpty()) {
		if (error) {
			*error = failure;
		}
		return nullptr;
	}
	return result;
}

SqlHistoryStore::SqlHistoryStore(QString connection, Ports::Cipher &cipher)
: _connection(std::move(connection))
, _cipher(cipher) {
}

SqlHistoryStore::~SqlHistoryStore() {
	Database(_connection).close();
	QSqlDatabase::removeDatabase(_connection);
}

QString SqlHistoryStore::migrate() {
	auto version = QSqlQuery(Database(_connection));
	if (!version.exec(u"PRAGMA user_version"_q) || !version.next()) {
		return u"cannot read the history schema version"_q;
	}
	const auto current = version.value(0).toInt();
	if (current == kSchemaVersion) {
		return QString();
	} else if (current != 0) {
		return u"unsupported history schema version %1"_q.arg(current);
	}
	auto database = Database(_connection);
	const auto created = database.transaction()
		&& Run(_connection, u"CREATE TABLE records ("
			"peer_id INTEGER NOT NULL, "
			"message_id INTEGER NOT NULL, "
			"revision INTEGER NOT NULL, "
			"kind INTEGER NOT NULL, "
			"topic_root_id INTEGER NOT NULL, "
			"recorded_at INTEGER NOT NULL, "
			"payload BLOB NOT NULL, "
			"PRIMARY KEY (peer_id, message_id, revision)"
			") WITHOUT ROWID"_q)
		&& Run(_connection, u"CREATE INDEX records_by_peer_time "
			"ON records (peer_id, kind, recorded_at)"_q)
		&& Run(_connection, u"PRAGMA user_version = %1"_q.arg(kSchemaVersion))
		&& database.commit();
	if (!created) {
		database.rollback();
		return u"cannot create the history schema"_q;
	}
	return QString();
}

bool SqlHistoryStore::save(const History::Record &record) {
	auto error = Codec::Error();
	if (!History::Validate(record, error, QString())) {
		return false;
	}
	auto query = QSqlQuery(Database(_connection));
	query.prepare(u"INSERT OR REPLACE INTO records "
		"(peer_id, message_id, revision, kind, topic_root_id, recorded_at, payload) "
		"VALUES (?, ?, ?, ?, ?, ?, ?)"_q);
	query.addBindValue(record.peerId);
	query.addBindValue(record.messageId);
	query.addBindValue(record.revision);
	query.addBindValue(int(record.kind));
	query.addBindValue(record.topicRootId);
	query.addBindValue(record.recordedAt);
	query.addBindValue(_cipher.encrypt(History::SerializeRecord(record)));
	return Run(query);
}

std::vector<History::Record> SqlHistoryStore::deleted(
		const Ports::HistoryQuery &query) {
	auto sql = u"SELECT payload FROM records WHERE peer_id = ? AND kind = ?"_q;
	if (query.topicRootId) {
		sql += u" AND topic_root_id = ?"_q;
	}
	if (query.recordedBefore) {
		sql += u" AND recorded_at < ?"_q;
	}
	if (query.minMessageId) {
		sql += u" AND message_id >= ?"_q;
	}
	if (query.maxMessageId) {
		sql += u" AND message_id <= ?"_q;
	}
	sql += u" ORDER BY recorded_at DESC, message_id DESC LIMIT ?"_q;
	auto select = QSqlQuery(Database(_connection));
	select.prepare(sql);
	select.addBindValue(query.peerId);
	select.addBindValue(int(History::RecordKind::Deleted));
	if (query.topicRootId) {
		select.addBindValue(*query.topicRootId);
	}
	if (query.recordedBefore) {
		select.addBindValue(*query.recordedBefore);
	}
	if (query.minMessageId) {
		select.addBindValue(*query.minMessageId);
	}
	if (query.maxMessageId) {
		select.addBindValue(*query.maxMessageId);
	}
	select.addBindValue(std::max(query.limit, 0));
	return Run(select) ? collect(select) : std::vector<History::Record>();
}

std::vector<History::Record> SqlHistoryStore::versions(
		qint64 peerId,
		qint64 messageId) {
	auto select = QSqlQuery(Database(_connection));
	select.prepare(u"SELECT payload FROM records "
		"WHERE peer_id = ? AND message_id = ? ORDER BY revision"_q);
	select.addBindValue(peerId);
	select.addBindValue(messageId);
	return Run(select) ? collect(select) : std::vector<History::Record>();
}

int SqlHistoryStore::nextRevision(qint64 peerId, qint64 messageId) {
	auto select = QSqlQuery(Database(_connection));
	select.prepare(u"SELECT COALESCE(MAX(revision) + 1, 0) FROM records "
		"WHERE peer_id = ? AND message_id = ?"_q);
	select.addBindValue(peerId);
	select.addBindValue(messageId);
	return (Run(select) && select.next()) ? select.value(0).toInt() : 0;
}

bool SqlHistoryStore::clearPeer(qint64 peerId) {
	auto query = QSqlQuery(Database(_connection));
	query.prepare(u"DELETE FROM records WHERE peer_id = ?"_q);
	query.addBindValue(peerId);
	return Run(query);
}

bool SqlHistoryStore::clearAll() {
	return Run(_connection, u"DELETE FROM records"_q)
		&& Run(_connection, u"VACUUM"_q);
}

bool SqlHistoryStore::prune(qint64 recordedBefore, int keepAtMost) {
	auto expired = QSqlQuery(Database(_connection));
	expired.prepare(u"DELETE FROM records WHERE recorded_at < ?"_q);
	expired.addBindValue(recordedBefore);
	auto excess = QSqlQuery(Database(_connection));
	excess.prepare(u"DELETE FROM records "
		"WHERE (peer_id, message_id, revision) IN ("
		"SELECT peer_id, message_id, revision FROM records "
		"ORDER BY recorded_at DESC LIMIT -1 OFFSET ?)"_q);
	excess.addBindValue(std::max(keepAtMost, 0));
	return Run(expired) && Run(excess);
}

int SqlHistoryStore::skippedRows() const {
	return _skippedRows;
}

std::vector<History::Record> SqlHistoryStore::collect(QSqlQuery &query) {
	auto result = std::vector<History::Record>();
	while (query.next()) {
		const auto plain = _cipher.decrypt(query.value(0).toByteArray());
		auto record = plain
			? History::ParseRecord(*plain)
			: std::optional<History::Record>();
		if (record) {
			result.push_back(std::move(*record));
		} else {
			++_skippedRows;
		}
	}
	return result;
}

} // namespace Serein::Adapters
