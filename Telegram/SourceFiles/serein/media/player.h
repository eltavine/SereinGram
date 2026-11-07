#pragma once

namespace Window { class SessionController; }

namespace Serein::Media {

void ShowExpandedPlayer(gsl::not_null<Window::SessionController*> controller);
void StartPlaybackTools();

} // namespace Serein::Media
