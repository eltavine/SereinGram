#pragma once

#include "serein/ports/chat_settings.h"

#include <span>

namespace Serein::Chats {

void ShowChatSettings(
	gsl::not_null<Window::SessionController*> controller,
	gsl::not_null<PeerData*> peer,
	std::span<const ChatSettingsRow> rows);

void AddLocalPinRow(const ChatSettingsContext &context);

} // namespace Serein::Chats
