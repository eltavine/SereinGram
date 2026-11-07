#include "serein/hooks/media/downloads.h"

#include "serein/media/options.h"
#include "serein/hooks/media/download_queue.h"
#include "serein/hooks/privacy/hidden.h"
#include "core/application.h"
#include "data/data_document.h"
#include "data/data_download_manager.h"
#include "data/data_media_types.h"
#include "history/history.h"
#include "history/history_item.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "ui/widgets/menu/menu_add_action_callback.h"

#include <QtCore/QFileInfo>

#include <array>

#include "styles/style_media_player.h"

namespace Serein::Media {
namespace {

[[nodiscard]] int Category(not_null<const HistoryItem*> item) {
	const auto media = item->media();
	const auto document = media ? media->document() : nullptr;
	if (!document) {
		return 4;
	} else if (document->song() || document->mimeString().startsWith(u"audio/"_q)) {
		return 2;
	} else if (document->isVideoFile() || document->mimeString().startsWith(u"video/"_q)) {
		return 3;
	}
	const auto suffix = QFileInfo(document->filename()).suffix().toLower();
	return (suffix == u"zip"_q || suffix == u"rar"_q || suffix == u"7z"_q
		|| suffix == u"gz"_q || suffix == u"bz2"_q || suffix == u"xz"_q
		|| suffix == u"tar"_q || suffix == u"zst"_q) ? 1 : 4;
}

} // namespace

bool DownloadVisible(
		not_null<Main::Session*> session,
		not_null<const HistoryItem*> item) {
	auto &options = ForAccount(session);
	const auto owner = &item->history()->session();
	if (Privacy::AccountHidden(&owner->account())
		|| (!options.Get(kDownloadsAllAccounts) && owner != session)) {
		return false;
	}
	const auto category = options.Get(kDownloadsCategory);
	return !category || category == Category(item);
}

void WatchDownloadFilters(
		not_null<Main::Session*> session,
		Fn<void()> refresh,
		rpl::lifetime &lifetime) {
	ForAccount(session).changes() | rpl::filter([](std::string_view key) {
		return key == kDownloadsAllAccounts.key || key == kDownloadsCategory.key;
	}) | rpl::on_next(std::move(refresh), lifetime);
}

void FillDownloadFilters(
		not_null<Main::Session*> session,
		const Ui::Menu::MenuCallback &addAction) {
	if (ForDevice().Get(kSequentialDownloads)) {
		addAction(tr::lng_serein_download_queue(tr::now), [=] {
			if (const auto window = session->tryResolveWindow()) session->sereinDownloads().show(window);
		});
	}
	const auto all = ForAccount(session).Get(kDownloadsAllAccounts);
	addAction(tr::lng_serein_downloads_all_accounts(tr::now), [=] {
		Expects(ForAccount(session).Set(kDownloadsAllAccounts, !all));
	}, all ? &st::mediaPlayerMenuCheck : nullptr);
	const auto titles = std::array{
		tr::lng_serein_downloads_all(tr::now),
		tr::lng_serein_downloads_archives(tr::now),
		tr::lng_serein_downloads_music(tr::now),
		tr::lng_serein_downloads_video(tr::now),
		tr::lng_serein_downloads_other(tr::now),
	};
	const auto selected = ForAccount(session).Get(kDownloadsCategory);
	for (auto i = 0; i != int(titles.size()); ++i) {
		addAction(titles[i], [=] {
			Expects(ForAccount(session).Set(kDownloadsCategory, i));
		}, selected == i ? &st::mediaPlayerMenuCheck : nullptr);
	}
}

std::vector<GlobalMsgId> VisibleDownloadIds(not_null<Main::Session*> session) {
	auto result = std::vector<GlobalMsgId>();
	auto &manager = Core::App().downloadManager();
	for (const auto entry : manager.loadedList()) {
		if (entry->object && DownloadVisible(session, entry->object->item)) {
			result.push_back(entry->object->item->globalId());
		}
	}
	for (const auto entry : manager.loadingList()) {
		if (DownloadVisible(session, entry->object.item)) {
			result.push_back(entry->object.item->globalId());
		}
	}
	return result;
}

} // namespace Serein::Media
