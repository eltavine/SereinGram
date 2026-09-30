#pragma once

#include <QtCore/QByteArray>
#include <gsl/pointers>

#include <optional>
#include <vector>

class History;
class HistoryItem;
class QPainter;
struct TextWithEntities;

namespace Main {
class Session;
} // namespace Main

namespace Serein::Ports {
class HistoryStore;
} // namespace Serein::Ports

namespace Serein::History {
struct Record;
} // namespace Serein::History

namespace Serein::Hooks {

[[nodiscard]] std::vector<gsl::not_null<HistoryItem*>> OnServerDeleted(
	std::vector<gsl::not_null<HistoryItem*>> items);
void OnBeforeEdition(
	gsl::not_null<HistoryItem*> item,
	const TextWithEntities &updated);
[[nodiscard]] std::vector<gsl::not_null<HistoryItem*>> OnExpiredMessages(
	std::vector<gsl::not_null<HistoryItem*>> items);
[[nodiscard]] bool KeepExpiredMedia(gsl::not_null<const HistoryItem*> item);
void OnHistorySliceAdded(gsl::not_null<::History*> history);

[[nodiscard]] Ports::HistoryStore *HistoryStoreFor(
	gsl::not_null<Main::Session*> session);
void PruneHistory(gsl::not_null<Main::Session*> session);
[[nodiscard]] bool ClearHistory(
	gsl::not_null<Main::Session*> session,
	long long peerId);
[[nodiscard]] std::optional<QByteArray> CachedMediaBytes(
	gsl::not_null<Main::Session*> session,
	const Serein::History::Record &record);
[[nodiscard]] bool OpenCachedMedia(
	gsl::not_null<Main::Session*> session,
	const Serein::History::Record &record);

class FadedPaint final {
public:
	FadedPaint(QPainter &p, gsl::not_null<const HistoryItem*> item);
	FadedPaint(const FadedPaint &) = delete;
	FadedPaint &operator=(const FadedPaint &) = delete;
	~FadedPaint();

private:
	QPainter &_p;
	double _opacity = 1.;
	bool _faded = false;

};

} // namespace Serein::Hooks
