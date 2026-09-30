#pragma once

#include "serein/core/options.h"
#include "data/data_types.h"

#include <QtCore/QJsonObject>
#include <QtGui/QImage>

class HistoryItem;
namespace Ui { class PopupMenu; }
namespace Window { class SessionController; }

namespace Serein::Snapshot {

[[nodiscard]] QJsonObject Defaults();
[[nodiscard]] bool Valid(const QJsonObject &value);
[[nodiscard]] bool Validate(const QByteArray &raw);
[[nodiscard]] std::variant<QImage, QString> Render(
	not_null<Window::SessionController*> controller,
	const MessageIdsList &ids,
	const QJsonObject &options,
	bool revealSpoilers);
void InsertAction(
	Ui::PopupMenu *menu,
	Window::SessionController *controller,
	HistoryItem *item,
	MessageIdsList selected);

inline const auto kSettings = Option<QByteArray>{
	"serein.snapshot", Scope::Device, QByteArray(),
	Category::Menu, "lng_serein_snapshot", 0, Validate };

inline void RegisterOptions(Registry &registry) {
	Expects(registry.Add(kSettings));
}

} // namespace Serein::Snapshot
