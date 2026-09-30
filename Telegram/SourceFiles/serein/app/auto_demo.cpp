#include "serein/app/auto_demo.h"

#include "serein/privacy/options.h"
#include "serein/privacy/recorders.h"
#include "base/algorithm.h"
#include "core/application.h"

#include <crl/crl_async.h>
#include <crl/crl_on_main.h>

#include <QtCore/QPointer>
#include <QtCore/QTimer>

namespace Serein::App {
namespace {

constexpr auto kCheckInterval = 5 * 1000;

class AutoDemo final : public QObject {
public:
	explicit AutoDemo(QObject *parent) : QObject(parent) {
		connect(&_timer, &QTimer::timeout, this, [=] { check(); });
		ForDevice().Value(Privacy::kAutoDemoMode) | rpl::on_next([=](
				bool enabled) {
			if (enabled) {
				_timer.start(kCheckInterval);
				check();
			} else {
				_timer.stop();
				apply(false);
			}
		}, _lifetime);
	}

private:
	void check() {
		if (_checking) {
			return;
		}
		_checking = true;
		crl::async([weak = QPointer<AutoDemo>(this)] {
			const auto recording = Privacy::RecorderRunning(
				Privacy::RunningProcessNames());
			crl::on_main([=] {
				if (const auto strong = weak.data()) {
					strong->checked(recording);
				}
			});
		});
	}

	void checked(bool recording) {
		_checking = false;
		if (ForDevice().Get(Privacy::kAutoDemoMode)) {
			apply(recording);
		}
	}

	void apply(bool recording) {
		auto &options = ForDevice();
		if (recording && !_active) {
			_active = true;
			_restore = !options.Get(Privacy::kDemoMode);
			if (_restore) {
				Expects(options.Set(Privacy::kDemoMode, true));
			}
		} else if (!recording && _active) {
			_active = false;
			if (base::take(_restore) && options.Get(Privacy::kDemoMode)) {
				Expects(options.Set(Privacy::kDemoMode, false));
			}
		}
	}

	QTimer _timer;
	rpl::lifetime _lifetime;
	bool _checking = false;
	bool _active = false;
	bool _restore = false;

};

} // namespace

void StartAutoDemoMode() {
	new AutoDemo(&Core::App());
}

} // namespace Serein::App
