#include "serein/adapters/qtsql/history_store.h"
#include "serein/features/history/media_store.h"
#include "serein/features/history/model/recorder.h"
#include "base/basic_types.h"

#include <QtCore/QFileInfo>
#include <QtCore/QTemporaryDir>

#include <stdexcept>

namespace {

void Require(bool value, const char *message) {
	if (!value) {
		throw std::runtime_error(message);
	}
}

class XorCipher final : public Serein::Ports::Cipher {
public:
	QByteArray encrypt(const QByteArray &plain) override {
		return u"sealed:"_q.toUtf8() + Flip(plain);
	}
	std::optional<QByteArray> decrypt(const QByteArray &sealed) override {
		const auto prefix = u"sealed:"_q.toUtf8();
		if (!sealed.startsWith(prefix)) {
			return std::nullopt;
		}
		return Flip(sealed.mid(prefix.size()));
	}

private:
	[[nodiscard]] static QByteArray Flip(QByteArray value) {
		for (auto &byte : value) {
			byte = char(byte ^ 0x5A);
		}
		return value;
	}
};

} // namespace

void TestCachedMedia() {
	using namespace Serein;
	using namespace Serein::HistoryFeature;
	auto directory = QTemporaryDir();
	auto cipher = XorCipher();
	auto store = Adapters::SqlHistoryStore::Open(
		directory.filePath(u"history.sqlite3"_q),
		cipher);
	Require(store != nullptr, "cached media store opens");
	const auto media = directory.filePath(u"serein_media"_q);
	const auto kept = CachedMediaPath(media, 555, 42);
	const auto orphan = CachedMediaPath(media, 555, 43);
	const auto other = CachedMediaPath(media, -777, 44);
	Require(QFileInfo(kept).fileName() == u"555_42.bin"_q
		&& QFileInfo(other).fileName() == u"-777_44.bin"_q,
		"cached media path");
	const auto bytes = QByteArray("\xff\xd8photo bytes", 13);
	Require(WriteCachedMedia(cipher, kept, bytes)
		&& WriteCachedMedia(cipher, orphan, bytes)
		&& WriteCachedMedia(cipher, other, bytes),
		"cached media written");
	{
		auto raw = QFile(kept);
		Require(raw.open(QIODevice::ReadOnly)
			&& !raw.readAll().contains("photo"),
			"cached media stored in plain text");
	}
	Require(ReadCachedMedia(cipher, kept) == bytes, "cached media read back");
	Require(!WriteCachedMedia(cipher, kept, QByteArray())
		&& !WriteCachedMedia(
			cipher,
			CachedMediaPath(media, 555, 45),
			QByteArray(kCachedMediaLimit + 1, 'x')),
		"empty or oversized cached media accepted");
	Require(!QFileInfo::exists(CachedMediaPath(media, 555, 45)),
		"oversized cached media left a file");

	auto recorder = Recorder(*store, [] { return qint64(1000000); });
	auto snapshot = Snapshot();
	snapshot.peerId = 555;
	snapshot.messageId = 42;
	snapshot.date = 1700000000;
	snapshot.fromPeerId = 99;
	snapshot.mediaSummary = u"Photo"_q;
	snapshot.cachedMediaName = u"photo.jpg"_q;
	auto policy = Policy();
	policy.saveDeleted = true;
	Require(recorder.recordDeleted(policy, snapshot), "media record saved");
	const auto records = store->deleted({ .peerId = 555 });
	Require(records.size() == 1
		&& records[0].cachedMediaName == u"photo.jpg"_q,
		"cached media name not saved");

	Require(RemoveOrphanedCachedMedia(media, *store) == 2
		&& QFileInfo::exists(kept)
		&& !QFileInfo::exists(orphan)
		&& !QFileInfo::exists(other),
		"orphaned cached media not swept");
	Require(WriteCachedMedia(cipher, other, bytes), "cached media rewritten");
	RemoveCachedMedia(media, 555);
	Require(!QFileInfo::exists(kept) && QFileInfo::exists(other),
		"cached media of one chat not removed alone");
	RemoveCachedMedia(media, 0);
	Require(!QFileInfo::exists(media), "cached media directory not removed");
}
