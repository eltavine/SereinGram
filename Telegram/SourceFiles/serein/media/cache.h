#pragma once

namespace Ui { class PopupMenu; }
namespace Window { class SessionController; }
class DocumentData;
class HistoryItem;

namespace Serein::Media {

void ClearDocumentCache(gsl::not_null<DocumentData*> document);
void InsertClearCacheAction(
	Ui::PopupMenu *menu,
	HistoryItem *item,
	Window::SessionController *controller);

} // namespace Serein::Media
