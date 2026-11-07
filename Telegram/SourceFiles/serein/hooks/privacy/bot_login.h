#pragma once

#include "base/basic_types.h"

class QWidget;
namespace Main { class Account; }
namespace Ui { class RpWidget; }
namespace Intro::details { class Step; struct Data; }

namespace Serein::Privacy {

void InstallBotLoginLink(
	gsl::not_null<Ui::RpWidget*> parent,
	gsl::not_null<Ui::RpWidget*> anchor,
	Fn<void()> open);
[[nodiscard]] Intro::details::Step *CreateBotLoginStep(
	QWidget *parent,
	gsl::not_null<Main::Account*> account,
	gsl::not_null<Intro::details::Data*> data);

} // namespace Serein::Privacy
