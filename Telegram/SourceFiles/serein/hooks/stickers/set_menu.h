#pragma once

#include <QtCore/QtGlobal>

#include <gsl/pointers>

#include <memory>

namespace ChatHelpers {
class Show;
} // namespace ChatHelpers

namespace Ui {
class PopupMenu;
} // namespace Ui

namespace Serein::Hooks {

void FillStickerSetMenu(
	gsl::not_null<Ui::PopupMenu*> menu,
	std::shared_ptr<ChatHelpers::Show> show,
	quint64 setId);

} // namespace Serein::Hooks
