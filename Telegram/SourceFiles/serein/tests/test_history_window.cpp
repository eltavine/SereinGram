#include "serein/adapters/qtsql/history_store.h"
#include "serein/features/history/model/window.h"
#include "base/basic_types.h"
#include "serein/tests/require.h"

#include <doctest/doctest.h>
#include <QtCore/QTemporaryDir>

#include <iostream>

namespace {

class PlainCipher final : public Serein::Ports::Cipher {
public:
	QByteArray encrypt(const QByteArray &plain) override {
		return plain;
	}
	std::optional<QByteArray> decrypt(const QByteArray &sealed) override {
		return sealed;
	}

};

[[nodiscard]] Serein::History::Record Deleted(qint64 messageId) {
	auto result = Serein::History::Record();
	result.kind = Serein::History::RecordKind::Deleted;
	result.peerId = 321;
	result.messageId = messageId;
	result.recordedAt = 5000 + messageId;
	result.text = u"deleted %1"_q.arg(messageId);
	return result;
}

[[nodiscard]] std::vector<qint64> Ids(
		const Serein::HistoryFeature::RecordsWindow &window) {
	auto result = std::vector<qint64>();
	for (const auto &record : window.records) {
		result.push_back(record.messageId);
	}
	return result;
}

[[nodiscard]] std::vector<qint64> Range(qint64 from, qint64 till) {
	auto result = std::vector<qint64>();
	for (auto id = from; id <= till; ++id) {
		result.push_back(id);
	}
	return result;
}

} // namespace

TEST_CASE("HistoryWindow") {
	using namespace Serein;
	using namespace Serein::HistoryFeature;
	auto directory = QTemporaryDir();
	auto cipher = PlainCipher();
	auto store = Adapters::SqlHistoryStore::Open(
		directory.filePath(u"history.sqlite3"_q),
		cipher);
	Require(store != nullptr, "window store opens");
	const auto filter = Ports::RecordsQuery{
		.peerId = 321,
		.kind = History::RecordKind::Deleted,
	};

	const auto empty = LoadWindow(*store, filter, { .limitBefore = 10 });
	Require(empty.records.empty()
		&& !empty.nearest
		&& empty.fullCount() == 0,
		"an empty chat gives an empty window");

	for (auto id = 1; id <= 250; ++id) {
		Require(store->save(Deleted(id)), "deleted record saves");
	}
	auto edit = Deleted(100);
	edit.kind = History::RecordKind::Edited;
	edit.revision = 1;
	Require(store->save(edit), "an edit of the same message saves");

	const auto newest = LoadWindow(*store, filter, {
		.limitBefore = 10,
		.limitAfter = 10,
	});
	Require(Ids(newest) == Range(240, 250)
		&& newest.nearest == Ports::RecordKey{ 250, 0 }
		&& newest.skippedBefore == 239
		&& newest.skippedAfter == 0
		&& newest.fullCount() == 250,
		"the default window ends at the newest record");

	const auto middle = LoadWindow(*store, filter, {
		.around = Ports::RecordKey{ 100, 0 },
		.limitBefore = 5,
		.limitAfter = 5,
	});
	Require(Ids(middle) == Range(95, 105)
		&& middle.nearest == Ports::RecordKey{ 100, 0 }
		&& middle.skippedBefore == 94
		&& middle.skippedAfter == 145,
		"a window around a record keeps the record and both sides");

	const auto start = LoadWindow(*store, filter, {
		.around = Ports::RecordKey{ 1, 0 },
		.limitBefore = 5,
		.limitAfter = 2,
	});
	Require(Ids(start) == Range(1, 3)
		&& start.skippedBefore == 0
		&& start.skippedAfter == 247,
		"a window at the oldest record reports nothing before it");

	const auto beyond = LoadWindow(*store, filter, {
		.around = Ports::RecordKey{ 999, 0 },
		.limitBefore = 3,
		.limitAfter = 3,
	});
	Require(Ids(beyond) == Range(248, 250)
		&& beyond.nearest == Ports::RecordKey{ 250, 0 }
		&& beyond.skippedAfter == 0,
		"an anchor past the end falls back to the newest records");

	auto versions = Ports::RecordsQuery{ .peerId = 321, .messageId = 100 };
	const auto history = LoadWindow(*store, versions, {
		.around = Ports::RecordKey{ 100, 0 },
		.limitAfter = 10,
	});
	Require(history.records.size() == 2
		&& history.records[1].kind == History::RecordKind::Edited
		&& history.fullCount() == 2,
		"a message filter windows over its versions only");
	std::cout << "PASS: Serein history window" << std::endl;
}
