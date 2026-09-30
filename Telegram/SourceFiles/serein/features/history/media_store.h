#pragma once

#include "serein/ports/history_store.h"

#include <QtCore/QString>

namespace Serein::HistoryFeature {

inline constexpr auto kCachedMediaLimit = 20 * 1024 * 1024;

[[nodiscard]] QString CachedMediaPath(
	const QString &directory,
	qint64 peerId,
	qint64 messageId);
[[nodiscard]] bool WriteCachedMedia(
	Ports::Cipher &cipher,
	const QString &path,
	const QByteArray &bytes);
[[nodiscard]] std::optional<QByteArray> ReadCachedMedia(
	Ports::Cipher &cipher,
	const QString &path);
void RemoveCachedMedia(const QString &directory, qint64 peerId);
int RemoveOrphanedCachedMedia(
	const QString &directory,
	Ports::HistoryStore &store);

} // namespace Serein::HistoryFeature
