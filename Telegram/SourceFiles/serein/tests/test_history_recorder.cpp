#include "serein/adapters/qtsql/history_store.h"
#include "serein/features/history/model/recorder.h"
#include "base/basic_types.h"

#include <QtCore/QTemporaryDir>

#include <iostream>
#include <stdexcept>

namespace {

void Require(bool value, const char *message) {
	if (!value) {
		throw std::runtime_error(message);
	}
}

class PlainCipher final : public Serein::Ports::Cipher {
public:
	QByteArray encrypt(const QByteArray &plain) override {
		return plain;
	}
	std::optional<QByteArray> decrypt(const QByteArray &sealed) override {
		return sealed;
	}

};

[[nodiscard]] Serein::HistoryFeature::Snapshot Message(qint64 id, bool bot = false) {
	auto result = Serein::HistoryFeature::Snapshot();
	result.peerId = 555;
	result.messageId = id;
	result.date = 1700000000;
	result.fromPeerId = 99;
	result.fromBot = bot;
	result.text = u"text %1"_q.arg(id);
	return result;
}

} // namespace

void TestHistoryRecorder() {
	using namespace Serein;
	using namespace Serein::HistoryFeature;
	auto directory = QTemporaryDir();
	auto cipher = PlainCipher();
	auto store = Adapters::SqlHistoryStore::Open(
		directory.filePath(u"history.sqlite3"_q),
		cipher);
	Require(store != nullptr, "recorder store opens");
	auto now = qint64(1000000);
	auto recorder = Recorder(*store, [&] { return now; });

	auto policy = Policy();
	Require(!recorder.recordDeleted(policy, Message(1)), "nothing is saved by default");
	policy.saveDeleted = true;
	policy.saveEdits = true;
	Require(recorder.recordDeleted(policy, Message(1)), "deleted message is saved");
	Require(!recorder.recordDeleted(policy, Message(2, true)), "bots are skipped by default");
	policy.includeBots = true;
	Require(recorder.recordDeleted(policy, Message(2, true)), "bots can be included");
	policy.excludedPeers = { 555 };
	Require(!recorder.recordDeleted(policy, Message(40)), "excluded chats are skipped");
	Require(!recorder.recordEdit(policy, Message(41)), "excluded chat edits are skipped");
	policy.excludedPeers.clear();
	using HistorySettings::ValidHistoryExclusions;
	Require(ValidHistoryExclusions({}), "empty exclusions rejected");
	Require(ValidHistoryExclusions(R"({"version":1,"peers":["555","777"]})"),
		"valid exclusions rejected");
	Require(!ValidHistoryExclusions(R"({"version":1,"peers":["0"]})"),
		"zero peer id accepted");
	Require(!ValidHistoryExclusions(R"({"version":1,"peers":["5","5"]})"),
		"duplicate peer ids accepted");
	Require(!ValidHistoryExclusions(R"({"version":1,"peers":["-5"]})"),
		"negative peer id accepted");
	Require(!ValidHistoryExclusions(R"({"peers":["5"]})"),
		"exclusions without a version accepted");
	Require(!recorder.recordDeleted(policy, Snapshot()), "empty snapshots are skipped");
	auto withFile = Message(42);
	withFile.localPath = u"/tmp/serein/file.pdf"_q;
	Require(recorder.recordDeleted(policy, withFile), "message with a file saved");
	const auto files = store->deleted({ .peerId = 555, .minMessageId = 42 });
	Require(files.size() == 1 && files[0].localPath == withFile.localPath,
		"deleted message file path not saved");

	Require(recorder.recordEdit(policy, Message(3)), "first edit is saved");
	now += 10;
	Require(recorder.recordEdit(policy, Message(3)), "second edit is saved");
	const auto versions = store->versions(555, 3);
	Require(versions.size() == 2
		&& versions[0].revision == 0
		&& versions[1].revision == 1
		&& versions[1].recordedAt == now,
		"edits get increasing revisions and timestamps");

	now += 3 * qint64(86400);
	policy.retentionDays = 2;
	Require(recorder.prune(policy), "pruning succeeds");
	Require(store->deleted({ .peerId = 555 }).empty() && store->versions(555, 3).empty(),
		"records older than the retention period are removed");
	std::cout << "PASS: Serein history recorder" << std::endl;
}
