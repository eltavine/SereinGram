#include "serein/hooks/iv.h"

#include "serein/core/options.h"
#include "serein/features/instant_view/translation.h"
#include "serein/schema/gen/settings/services.h"
#include "base/flat_map.h"
#include "base/weak_ptr.h"
#include "core/application.h"
#include "core/core_settings.h"
#include "data/data_session.h"
#include "data/data_web_page.h"
#include "iv/iv_data.h"
#include "iv/iv_rich_page.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "ui/toast/toast.h"
#include "ui/widgets/popup_menu.h"
#include "styles/style_menu_icons.h"

#include <QtCore/QPointer>

namespace Serein::Hooks::Iv {
namespace {

struct Page {
	std::shared_ptr<const ::Iv::RichPage> source;
	std::shared_ptr<const ::Iv::RichPage> translated;
	bool showTranslated = false;
	bool loading = false;
};

struct SessionState {
	base::flat_map<uint64, Page> pages;
	rpl::event_stream<uint64> refreshes;
};

struct Shown {
	base::weak_ptr<Main::Session> session;
	uint64 pageId = 0;
};

[[nodiscard]] bool Enabled() {
	return ForDevice().Get(Serein::ServiceSettings::kInstantViewTranslation);
}

[[nodiscard]] Shown &CurrentlyShown() {
	static auto result = Shown();
	return result;
}

[[nodiscard]] SessionState &StateFor(not_null<Main::Session*> session) {
	static auto states = base::flat_map<
		not_null<Main::Session*>,
		std::unique_ptr<SessionState>>();
	auto &result = states[session];
	if (!result) {
		result = std::make_unique<SessionState>();
		session->lifetime().add([=] {
			states.remove(session);
		});
	}
	return *result;
}

void ShowToast(const QPointer<QWidget> &parent, const QString &text) {
	if (parent) {
		Ui::Toast::Show(parent.data(), text);
	}
}

void Finished(
		const base::weak_ptr<Main::Session> &weak,
		uint64 pageId,
		const std::shared_ptr<const ::Iv::RichPage> &source,
		const QPointer<QWidget> &parent,
		IvTranslation::Result result) {
	const auto session = weak.get();
	if (!session) {
		return;
	}
	auto &state = StateFor(session);
	const auto i = state.pages.find(pageId);
	if (i == end(state.pages) || i->second.source != source) {
		return;
	}
	i->second.loading = false;
	if (result.page) {
		i->second.translated = std::move(result.page);
		i->second.showTranslated = true;
		state.refreshes.fire_copy(pageId);
	}
	if (!result.error.isEmpty()) {
		LOG(("Serein: Instant View translation stopped: %1"
			).arg(result.error));
		ShowToast(parent, i->second.translated
			? tr::lng_serein_iv_translate_partial(tr::now)
			: tr::lng_serein_iv_translate_failed(tr::now));
	}
}

void Toggle(
		not_null<Main::Session*> session,
		uint64 pageId,
		QPointer<QWidget> parent) {
	const auto data = session->data().webpage(pageId)->iv.get();
	if (!data || !data->richPage()) {
		return;
	}
	auto &state = StateFor(session);
	auto &entry = state.pages[pageId];
	if (entry.source != data->richPage()) {
		entry = Page{ .source = data->richPage() };
	}
	if (entry.showTranslated || entry.translated) {
		entry.showTranslated = !entry.showTranslated;
		state.refreshes.fire_copy(pageId);
		return;
	} else if (entry.loading) {
		return;
	}
	entry.loading = true;
	ShowToast(parent, tr::lng_serein_iv_translating(tr::now));
	const auto weak = base::make_weak(session);
	const auto source = entry.source;
	IvTranslation::TranslatePage(
		session,
		source,
		Core::App().settings().translateTo(),
		[=](IvTranslation::Result result) {
			Finished(weak, pageId, source, parent, std::move(result));
		});
}

} // namespace

std::shared_ptr<const ::Iv::RichPage> DisplayedPage(
		not_null<Main::Session*> session,
		not_null<::Iv::Data*> data) {
	const auto &original = data->richPage();
	CurrentlyShown() = {
		.session = base::make_weak(session),
		.pageId = data->pageId(),
	};
	if (!Enabled()) {
		return original;
	}
	auto &pages = StateFor(session).pages;
	const auto i = pages.find(data->pageId());
	if (i == end(pages)) {
		return original;
	} else if (i->second.source != original) {
		pages.erase(i);
		return original;
	}
	return (i->second.showTranslated && i->second.translated)
		? i->second.translated
		: original;
}

void OnPageRefresh(
		not_null<Main::Session*> session,
		rpl::lifetime &lifetime,
		Fn<void(not_null<::Iv::Data*> data)> callback) {
	StateFor(session).refreshes.events(
	) | rpl::on_next([=](uint64 pageId) {
		if (const auto data = session->data().webpage(pageId)->iv.get()) {
			callback(data);
		}
	}, lifetime);
}

void FillMenu(not_null<Ui::PopupMenu*> menu, uint64 pageId) {
	const auto &shown = CurrentlyShown();
	const auto session = shown.session.get();
	if (!pageId || shown.pageId != pageId || !session || !Enabled()) {
		return;
	}
	const auto data = session->data().webpage(pageId)->iv.get();
	if (!data || !data->richPage()) {
		return;
	}
	const auto &pages = StateFor(session).pages;
	const auto i = pages.find(pageId);
	const auto translated = (i != end(pages))
		&& i->second.showTranslated
		&& (i->second.source == data->richPage());
	const auto parent = QPointer<QWidget>(menu->parentWidget());
	const auto weak = shown.session;
	menu->addAction(
		(translated
			? tr::lng_serein_iv_show_original
			: tr::lng_serein_iv_translate)(tr::now),
		[=] {
			if (const auto strong = weak.get()) {
				Toggle(strong, pageId, parent);
			}
		},
		&st::menuIconTranslate);
}

} // namespace Serein::Hooks::Iv
