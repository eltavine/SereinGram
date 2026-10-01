#include "serein/interface/global_shortcut.h"

#include "base/algorithm.h"
#include "base/global_shortcuts.h"
#include "base/platform/base_platform_info.h"
#include "core/application.h"
#include "lang/lang_keys.h"
#include "serein/core/options.h"
#include "serein/schema/gen/settings/interface.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/labels.h"
#include "mainwindow.h"
#include "window/window_controller.h"
#include "styles/style_layers.h"

#include <crl/crl_on_main.h>

namespace Serein::Interface {
namespace {

void ToggleWindow() {
	const auto window = Core::App().activePrimaryWindow();
	if (!window) {
		return;
	}
	const auto widget = window->widget();
	if (widget->isActive() && !widget->isMinimized()) {
		window->minimize();
	} else {
		window->activate();
	}
}

class Watcher final : public QObject {
public:
	explicit Watcher(QObject *parent) : QObject(parent) {
		ForDevice().Value(
			kGlobalShortcut
		) | rpl::on_next([=](const QByteArray &value) {
			apply(value);
		}, _lifetime);
	}

	[[nodiscard]] base::GlobalShortcutManager *manager() {
		if (!_manager) {
			_manager = base::CreateGlobalShortcutManager();
		}
		return _manager.get();
	}

private:
	void apply(const QByteArray &value) {
		if (_manager && _shortcut) {
			_manager->stopWatching(base::take(_shortcut));
		}
		if (value.isEmpty()
			|| !base::GlobalShortcutsAvailable()
			|| !base::GlobalShortcutsAllowed()) {
			return;
		}
		_shortcut = manager()->shortcutFromSerialized(value);
		if (_shortcut) {
			_manager->startWatching(_shortcut, [=](bool pressed) {
				if (pressed) {
					crl::on_main(this, ToggleWindow);
				}
			});
		}
	}

	std::unique_ptr<base::GlobalShortcutManager> _manager;
	base::GlobalShortcut _shortcut;
	rpl::lifetime _lifetime;

};

Watcher *Instance = nullptr;

void UnavailableBox(not_null<Ui::GenericBox*> box) {
	auto text = tr::lng_serein_global_shortcut_unsupported();
#ifdef Q_OS_MAC
	if (base::GlobalShortcutsAvailable()) {
		text = tr::lng_serein_global_shortcut_permission();
		box->addButton(tr::lng_serein_global_shortcut_settings(), [] {
			if (::Platform::IsMac10_15OrGreater()) {
				::Platform::OpenInputMonitoringPrivacySettings();
			} else {
				::Platform::OpenAccessibilityPrivacySettings();
			}
		});
	}
#endif // Q_OS_MAC
	box->addRow(object_ptr<Ui::FlatLabel>(box, std::move(text), st::boxLabel));
	box->addButton(tr::lng_close(), [=] { box->closeBox(); });
}

} // namespace

void StartGlobalShortcut() {
	Instance = new Watcher(&Core::App());
}

void GlobalShortcutBox(not_null<Ui::GenericBox*> box) {
	box->setTitle(tr::lng_serein_global_shortcut());
	if (!Instance
		|| !base::GlobalShortcutsAvailable()
		|| !base::GlobalShortcutsAllowed()) {
		UnavailableBox(box);
		return;
	}
	struct State {
		rpl::variable<QString> text;
		bool recording = false;
	};
	const auto state = box->lifetime().make_state<State>();
	const auto manager = Instance->manager();
	const auto describe = [=](const QByteArray &value) {
		const auto shortcut = value.isEmpty()
			? nullptr
			: manager->shortcutFromSerialized(value);
		return shortcut
			? shortcut->toDisplayString()
			: tr::lng_serein_global_shortcut_none(tr::now);
	};
	state->text = describe(ForDevice().Get(kGlobalShortcut));
	box->addRow(object_ptr<Ui::FlatLabel>(
		box,
		state->text.value(),
		st::boxTitle));
	box->addRow(object_ptr<Ui::FlatLabel>(
		box,
		tr::lng_serein_global_shortcut_about(),
		st::boxLabel));
	const auto stop = [=] {
		if (base::take(state->recording)) {
			manager->stopRecording();
		}
	};
	box->addButton(tr::lng_serein_global_shortcut_record(), [=] {
		if (state->recording) {
			return;
		}
		state->recording = true;
		state->text = tr::lng_serein_global_shortcut_press(tr::now);
		manager->startRecording([=](base::GlobalShortcut shortcut) {
			crl::on_main(box, [=] {
				state->text = shortcut->toDisplayString();
			});
		}, [=](base::GlobalShortcut shortcut) {
			crl::on_main(box, [=] {
				state->recording = false;
				const auto value = shortcut
					? shortcut->serialize()
					: QByteArray();
				Expects(ForDevice().Set(kGlobalShortcut, value));
				state->text = describe(value);
			});
		});
	});
	box->addButton(tr::lng_serein_global_shortcut_clear(), [=] {
		stop();
		Expects(ForDevice().Set(kGlobalShortcut, QByteArray()));
		state->text = describe(QByteArray());
	});
	box->addButton(tr::lng_close(), [=] { box->closeBox(); });
	box->boxClosing() | rpl::on_next(stop, box->lifetime());
}

} // namespace Serein::Interface
