#include "serein/app/inspector.h"

#include "serein/features/history/wire_cache.h"
#include "serein/features/inspector/sources.h"
#include "serein/menu/details.h"

namespace Serein::App {

void RegisterInspector() {
	Inspector::SetSavedProvider([](gsl::not_null<const HistoryItem*> item) {
		return HistoryFeature::CapturedWire(item);
	});
	Inspector::AddFactsProvider([](gsl::not_null<HistoryItem*> item) {
		return Menu::StickerFacts(item);
	});
}

} // namespace Serein::App
