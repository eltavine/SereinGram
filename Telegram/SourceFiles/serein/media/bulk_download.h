#pragma once

#include "serein/ports/chat_settings.h"

namespace Serein::Media {

void DownloadChatMedia(
	gsl::not_null<Window::SessionController*> controller,
	gsl::not_null<PeerData*> peer);
void AddMediaDownloadRow(const ChatSettingsContext &context);

} // namespace Serein::Media
