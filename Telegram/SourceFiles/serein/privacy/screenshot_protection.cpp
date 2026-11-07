#include "serein/privacy/screenshot_protection.h"

#include "core/application.h"
#include "core/core_screenshot_protection.h"
#include "serein/privacy/options.h"

#include <QtCore/QEvent>
#include <QtWidgets/QApplication>

#include <map>

namespace Serein::Privacy {
namespace {

class DemoModeProtection final : public QObject {
public:
	DemoModeProtection(
		not_null<QObject*> parent,
		not_null<Core::ScreenshotProtection*> protection)
	: QObject(parent)
	, _protection(protection) {
		qApp->installEventFilter(this);
		for (const auto window : QApplication::topLevelWidgets()) {
			watch(window);
		}
	}

	~DemoModeProtection() {
		qApp->removeEventFilter(this);
	}

protected:
	bool eventFilter(QObject *watched, QEvent *event) override {
		const auto type = event->type();
		if (type == QEvent::Show || type == QEvent::WinIdChange) {
			if (const auto widget = qobject_cast<QWidget*>(watched)) {
				if (widget->isWindow()) {
					watch(widget);
				}
			}
		}
		return QObject::eventFilter(watched, event);
	}

private:
	void watch(not_null<QWidget*> window) {
		const auto protection = _protection.get();
		if (!protection) {
			return;
		}
		const auto [i, added] = _lifetimes.try_emplace(window.get());
		if (!added) {
			return;
		}
		connect(window.get(), &QObject::destroyed, this, [=] {
			_lifetimes.erase(window.get());
		});
		protection->addContentReason(
			window,
			ForDevice().Value(kDemoMode),
			i->second);
	}

	base::weak_ptr<Core::ScreenshotProtection> _protection;
	std::map<QWidget*, rpl::lifetime> _lifetimes;

};

} // namespace

void StartScreenshotProtection() {
	auto &app = Core::App();
	new DemoModeProtection(&app, &app.screenshotProtection());
}

} // namespace Serein::Privacy
