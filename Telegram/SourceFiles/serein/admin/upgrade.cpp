#include "serein/admin/upgrade.h"

#include "apiwrap.h"
#include "data/data_channel.h"
#include "data/data_chat.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "ui/boxes/confirm_box.h"
#include "window/window_session_controller.h"

namespace Serein::Admin {

bool CanUpgradeToSupergroup(gsl::not_null<PeerData*> peer) {
	const auto chat = peer->asChat();
	return chat && chat->amCreator() && !chat->migrateTo();
}

void ConfirmUpgradeToSupergroup(
		gsl::not_null<Window::SessionController*> controller,
		gsl::not_null<PeerData*> peer) {
	const auto chat = peer->asChat();
	if (!chat) {
		return;
	}
	const auto weak = base::make_weak(controller);
	controller->show(Ui::MakeConfirmBox({
		.text = tr::lng_serein_upgrade_supergroup_sure(tr::now),
		.confirmed = [=](Fn<void()> &&close) {
			close();
			chat->session().api().migrateChat(chat, [=](
					not_null<ChannelData*> channel) {
				if (const auto strong = weak.get()) {
					strong->showPeerHistory(
						channel,
						Window::SectionShow::Way::ClearStack);
					strong->uiShow()->showToast(
						tr::lng_serein_upgrade_supergroup_done(tr::now));
				}
			}, [=](const QString &error) {
				if (const auto strong = weak.get()) {
					strong->uiShow()->showToast(error);
				}
			});
		},
		.confirmText = tr::lng_serein_upgrade_supergroup_confirm(),
	}));
}

} // namespace Serein::Admin
