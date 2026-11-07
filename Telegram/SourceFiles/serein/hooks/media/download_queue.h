#pragma once

#include "base/basic_types.h"

#include <memory>
#include <vector>

struct FullMsgId;

namespace Main { class Session; }
namespace Window { class SessionController; }

namespace Serein::Hooks {

class DownloadQueue final {
public:
	explicit DownloadQueue(gsl::not_null<Main::Session*> session);
	~DownloadQueue();
	void enqueue(std::vector<FullMsgId> ids);
	void show(gsl::not_null<Window::SessionController*> controller);

private:
	class Impl;
	std::unique_ptr<Impl> _impl;

};

} // namespace Serein::Hooks
