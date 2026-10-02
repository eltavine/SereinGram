#pragma once

#include "serein/features/history/model/recorder.h"

#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <gsl/pointers>

#include <optional>

class HistoryItem;

namespace Serein::HistoryFeature {

struct CachedMedia {
	QByteArray bytes;
	QString name;
};

[[nodiscard]] QString SafeFileName(const QString &name);
[[nodiscard]] Snapshot TakeSnapshot(gsl::not_null<HistoryItem*> item);
[[nodiscard]] std::optional<CachedMedia> CaptureCachedMedia(
	gsl::not_null<HistoryItem*> item);

} // namespace Serein::HistoryFeature
