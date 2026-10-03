#pragma once

namespace Window {
class SessionController;
} // namespace Window

namespace Serein::Presets {

void ShowPresets(not_null<Window::SessionController*> controller);
void OfferPresetsOnce(not_null<Window::SessionController*> controller);

} // namespace Serein::Presets
