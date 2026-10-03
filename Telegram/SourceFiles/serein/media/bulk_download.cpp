#include "serein/media/bulk_download.h"

#include "core/application.h"
#include "data/data_peer.h"
#include "export/export_manager.h"
#include "export/export_settings.h"
#include "export/view/export_view_settings.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "storage/storage_account.h"
#include "ui/widgets/buttons.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"

#include "styles/style_settings.h"

namespace Serein::Media {

void DownloadChatMedia(
		not_null<Window::SessionController*> controller,
		not_null<PeerData*> peer) {
	auto &local = controller->session().local();
	const auto saved = local.readExportSettings();
	auto media = saved;
	media.media.types = Export::MediaSettings::Type::AllMask;
	media.media.sizeLimit = Export::View::SizeLimitByIndex(
		Export::View::kSizeValueCount - 1);
	local.writeExportSettings(media);
	Core::App().exportManager().start(peer);
	local.writeExportSettings(saved);
}

void AddMediaDownloadRow(const ChatSettingsContext &context) {
	const auto controller = context.controller;
	const auto peer = context.peer;
	if (!peer->canExportChatHistory()) {
		return;
	}
	const auto button = context.container->add(
		object_ptr<Ui::SettingsButton>(
			context.container,
			tr::lng_serein_chat_download_media(),
			st::settingsButtonNoIcon));
	button->setClickedCallback([=] { DownloadChatMedia(controller, peer); });
}

} // namespace Serein::Media
