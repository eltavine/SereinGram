#include "serein/features/history/media_store.h"

#include "base/basic_types.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QSaveFile>

namespace Serein::HistoryFeature {
namespace {

constexpr auto kSuffix = QLatin1String(".bin");
constexpr auto kSealOverhead = 1024;

} // namespace

QString CachedMediaPath(
		const QString &directory,
		qint64 peerId,
		qint64 messageId) {
	return directory
		+ u'/'
		+ QString::number(peerId)
		+ u'_'
		+ QString::number(messageId)
		+ kSuffix;
}

bool WriteCachedMedia(
		Ports::Cipher &cipher,
		const QString &path,
		const QByteArray &bytes) {
	if (bytes.isEmpty()
		|| bytes.size() > kCachedMediaLimit
		|| !QDir().mkpath(QFileInfo(path).absolutePath())) {
		return false;
	}
	const auto sealed = cipher.encrypt(bytes);
	auto file = QSaveFile(path);
	if (sealed.isEmpty() || !file.open(QIODevice::WriteOnly)) {
		return false;
	} else if (file.write(sealed) != sealed.size()) {
		file.cancelWriting();
		return false;
	}
	return file.commit();
}

std::optional<QByteArray> ReadCachedMedia(
		Ports::Cipher &cipher,
		const QString &path) {
	auto file = QFile(path);
	if (file.size() > kCachedMediaLimit + kSealOverhead
		|| !file.open(QIODevice::ReadOnly)) {
		return std::nullopt;
	}
	return cipher.decrypt(file.readAll());
}

void RemoveCachedMedia(const QString &directory, qint64 peerId) {
	auto dir = QDir(directory);
	if (!peerId) {
		dir.removeRecursively();
		return;
	}
	const auto pattern = QString::number(peerId) + u"_*"_q + kSuffix;
	for (const auto &name : dir.entryList({ pattern }, QDir::Files)) {
		dir.remove(name);
	}
}

int RemoveOrphanedCachedMedia(
		const QString &directory,
		Ports::HistoryStore &store) {
	auto dir = QDir(directory);
	auto removed = 0;
	const auto pattern = u"*"_q + kSuffix;
	for (const auto &name : dir.entryList({ pattern }, QDir::Files)) {
		const auto parts = QStringView(name).chopped(kSuffix.size()).split(u'_');
		auto peerValid = false;
		auto messageValid = false;
		const auto peerId = (parts.size() == 2)
			? parts[0].toLongLong(&peerValid)
			: 0;
		const auto messageId = (parts.size() == 2)
			? parts[1].toLongLong(&messageValid)
			: 0;
		const auto kept = peerValid
			&& messageValid
			&& !store.deleted({
				.peerId = peerId,
				.minMessageId = messageId,
				.maxMessageId = messageId,
				.limit = 1,
			}).empty();
		if (!kept && dir.remove(name)) {
			++removed;
		}
	}
	return removed;
}

} // namespace Serein::HistoryFeature
