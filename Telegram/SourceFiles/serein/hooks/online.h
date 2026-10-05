#pragma once

#include "base/basic_types.h"

#include <QtCore/QString>
#include <gsl/pointers>

#include <memory>
#include <optional>

class ClickHandler;
class PeerData;
class QPainter;

namespace Data {
class LastseenStatus;
} // namespace Data

namespace Dialogs {
class Entry;
} // namespace Dialogs

namespace Main {
class Session;
} // namespace Main

namespace Serein::Hooks::Online {

[[nodiscard]] QString RowPrefix(gsl::not_null<const Dialogs::Entry*> entry);
[[nodiscard]] std::optional<QString> StatusText(
	const Data::LastseenStatus &status,
	TimeId now);
[[nodiscard]] QString LinkTooltip(
	gsl::not_null<Main::Session*> session,
	const std::shared_ptr<ClickHandler> &link);
void PaintSender(
	QPainter &p,
	gsl::not_null<PeerData*> from,
	int top,
	int outerWidth);

} // namespace Serein::Hooks::Online
