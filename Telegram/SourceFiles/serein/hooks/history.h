#pragma once

#include <QtCore/QString>
#include <gsl/pointers>

#include <vector>

class History;
class HistoryItem;
class QPainter;
struct TextWithEntities;

namespace Serein::Hooks {

[[nodiscard]] std::vector<gsl::not_null<HistoryItem*>> OnServerDeleted(
	std::vector<gsl::not_null<HistoryItem*>> items);
void OnBeforeEdition(
	gsl::not_null<HistoryItem*> item,
	const TextWithEntities &updated);
template <typename Message>
void OnMessageReceived(
	gsl::not_null<::History*> history,
	qint64 id,
	const Message &message);
template <typename Message>
void OnMessageEdited(
	gsl::not_null<HistoryItem*> item,
	const Message &message);
[[nodiscard]] std::vector<gsl::not_null<HistoryItem*>> OnExpiredMessages(
	std::vector<gsl::not_null<HistoryItem*>> items);
[[nodiscard]] bool KeepExpiredMedia(gsl::not_null<const HistoryItem*> item);
void OnHistorySliceAdded(gsl::not_null<::History*> history);
[[nodiscard]] bool AutoTranslate(gsl::not_null<::History*> history);
[[nodiscard]] QString DeletedReplyText(
	gsl::not_null<::History*> history,
	qint64 messageId,
	const QString &fallback);

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
