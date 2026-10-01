#include "serein/hooks/stickers/set_menu.h"

#include "serein/app/user_lookup.h"
#include "serein/features/stickers/model/owner.h"
#include "chat_helpers/compose/compose_show.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "data/data_user.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "ui/widgets/popup_menu.h"
#include "window/window_session_controller.h"
#include "styles/style_menu_icons.h"

#include <QtGui/QClipboard>
#include <QtGui/QGuiApplication>

namespace Serein::Hooks {

void FillStickerSetMenu(
		gsl::not_null<Ui::PopupMenu*> menu,
		std::shared_ptr<ChatHelpers::Show> show,
		quint64 setId) {
	const auto ownerId = Stickers::SetOwnerId(setId);
	if (!ownerId) {
		return;
	}
	menu->addAction(tr::lng_serein_sticker_set_author(tr::now), [=] {
		const auto open = [=](PeerData *peer, bool searched) {
			if (peer) {
				if (const auto window = show->resolveWindow()) {
					window->showPeerInfo(peer);
					return;
				}
			}
			QGuiApplication::clipboard()->setText(QString::number(ownerId));
			show->showToast(searched
				? tr::lng_serein_sticker_set_author_not_found(tr::now)
				: tr::lng_serein_sticker_set_author_copied(tr::now));
		};
		const auto session = &show->session();
		const auto id = UserId(ownerId);
		if (const auto peer = session->data().peerLoaded(peerFromUser(id))) {
			open(peer, false);
		} else if (HasUserLookup()) {
			show->showToast(tr::lng_serein_sticker_set_author_searching(tr::now));
			LookupUser(session, id, [=](UserData *user) {
				open(user, true);
			});
		} else {
			open(nullptr, false);
		}
	}, &st::menuIconProfile);
}

} // namespace Serein::Hooks
