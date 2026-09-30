#include "serein/features/history/model/recorder.h"

#include <limits>

namespace Serein::HistoryFeature {
namespace {

constexpr auto kSecondsPerDay = qint64(86400);

[[nodiscard]] std::set<qint64> ReadExclusions(Options &account) {
	auto result = std::set<qint64>();
	const auto parsed = ParseHistoryExclusions(
		account.Get(HistorySettings::kHistoryExcludedPeers));
	for (const auto &peer : parsed ? parsed->peers : std::vector<QString>()) {
		auto ok = false;
		if (const auto id = peer.toLongLong(&ok); ok) {
			result.emplace(id);
		}
	}
	return result;
}

[[nodiscard]] bool Wanted(const Policy &policy, const Snapshot &snapshot) {
	return (policy.includeBots || !snapshot.fromBot)
		&& !policy.excludedPeers.contains(snapshot.peerId)
		&& snapshot.peerId != 0
		&& snapshot.messageId > 0
		&& (!snapshot.text.isEmpty() || !snapshot.mediaSummary.isEmpty());
}

} // namespace

Policy Read(Options &account) {
	using namespace HistorySettings;
	return {
		.saveDeleted = account.Get(kHistorySaveDeleted),
		.saveEdits = account.Get(kHistorySaveEdits),
		.includeBots = account.Get(kHistoryIncludeBots),
		.retentionDays = account.Get(kHistoryRetentionDays),
		.maxRecords = account.Get(kHistoryMaxRecords),
		.excludedPeers = ReadExclusions(account),
	};
}

bool Excluded(Options &account, qint64 peerId) {
	return ReadExclusions(account).contains(peerId);
}

bool SetExcluded(Options &account, qint64 peerId, bool excluded) {
	auto peers = ReadExclusions(account);
	if (excluded) {
		peers.emplace(peerId);
	} else {
		peers.erase(peerId);
	}
	auto value = HistoryExclusions();
	for (const auto peer : peers) {
		value.peers.push_back(QString::number(peer));
	}
	return account.Set(
		HistorySettings::kHistoryExcludedPeers,
		value.peers.empty() ? QByteArray() : SerializeHistoryExclusions(value));
}

Recorder::Recorder(Ports::HistoryStore &store, std::function<qint64()> now)
: _store(store)
, _now(std::move(now)) {
}

bool Recorder::recordDeleted(const Policy &policy, const Snapshot &snapshot) {
	return policy.saveDeleted
		&& Wanted(policy, snapshot)
		&& record(History::RecordKind::Deleted, snapshot);
}

bool Recorder::recordEdit(const Policy &policy, const Snapshot &before) {
	return policy.saveEdits
		&& Wanted(policy, before)
		&& record(History::RecordKind::Edited, before);
}

bool Recorder::prune(const Policy &policy) {
	const auto cutoff = (policy.retentionDays > 0)
		? (_now() - policy.retentionDays * kSecondsPerDay)
		: qint64(0);
	const auto keep = (policy.maxRecords > 0)
		? policy.maxRecords
		: std::numeric_limits<int>::max();
	return _store.prune(cutoff, keep);
}

bool Recorder::record(History::RecordKind kind, const Snapshot &snapshot) {
	auto record = History::Record();
	record.kind = kind;
	record.peerId = snapshot.peerId;
	record.messageId = snapshot.messageId;
	record.topicRootId = snapshot.topicRootId;
	record.revision = _store.nextRevision(snapshot.peerId, snapshot.messageId);
	record.date = snapshot.date;
	record.recordedAt = _now();
	record.fromPeerId = snapshot.fromPeerId;
	record.text = snapshot.text;
	record.entities = snapshot.entities;
	record.mediaSummary = snapshot.mediaSummary;
	return _store.save(record);
}

} // namespace Serein::HistoryFeature

namespace Serein::HistorySettings {

bool ValidHistoryExclusions(const QByteArray &value) {
	return value.isEmpty()
		|| HistoryFeature::ParseHistoryExclusions(value).has_value();
}

} // namespace Serein::HistorySettings
