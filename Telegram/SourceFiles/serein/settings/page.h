#pragma once

#include "serein/settings/lock.h"
#include "settings/settings_common_session.h"
#include "ui/ui_utility.h"
#include "ui/wrap/vertical_layout.h"

namespace Serein {

template <typename Derived>
class Page : public ::Settings::Section<Derived> {
public:
	Page(
		QWidget *parent,
		not_null<Window::SessionController*> controller)
	: ::Settings::Section<Derived>(parent, controller) {
		const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);
		GuardSettings(content, [=, this] {
			this->build(content, Derived::kBuild);
		});
		Ui::ResizeFitChild(this, content);
	}

};

} // namespace Serein
