#pragma once

#include "base/basic_types.h"
#include "serein/ports/chat_settings.h"

#include <memory>
#include <gsl/pointers>

class PeerData;

namespace Main {
class Session;
class SessionShow;
} // namespace Main

namespace Serein {

[[nodiscard]] QString SendTranslationName(gsl::not_null<PeerData*> peer);
[[nodiscard]] std::vector<gsl::not_null<PeerData*>> SendTranslationPeers(
	gsl::not_null<Main::Session*> session);
void ChooseSendTranslation(
	std::shared_ptr<Main::SessionShow> show,
	gsl::not_null<PeerData*> peer);
void AddSendTranslationRow(const ChatSettingsContext &context);

} // namespace Serein
