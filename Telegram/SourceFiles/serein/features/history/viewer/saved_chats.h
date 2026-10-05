#pragma once

#include "boxes/peer_list_box.h"

namespace Window {
class SessionController;
} // namespace Window

namespace Serein::HistoryFeature::Viewer {

class SavedChatsController final : public PeerListController {
public:
	explicit SavedChatsController(
		not_null<Window::SessionController*> window);

	Main::Session &session() const override;
	void prepare() override;
	void rowClicked(not_null<PeerListRow*> row) override;

private:
	const not_null<Window::SessionController*> _window;

};

} // namespace Serein::HistoryFeature::Viewer
