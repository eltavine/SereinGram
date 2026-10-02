#pragma once

#include "serein/core/options.h"
#include "serein/schema/gen/settings/snapshot.h"
#include "serein/snapshot/rules.h"
#include "data/data_types.h"

#include <QtGui/QImage>

class HistoryItem;
namespace Ui { class PopupMenu; }
namespace Window { class SessionController; }

namespace Serein::Snapshot {

[[nodiscard]] std::variant<QImage, QString> Render(
	not_null<Window::SessionController*> controller,
	const MessageIdsList &ids,
	const SnapshotConfig &options,
	bool revealSpoilers);
void InsertAction(
	Ui::PopupMenu *menu,
	Window::SessionController *controller,
	HistoryItem *item,
	MessageIdsList selected);

} // namespace Serein::Snapshot
