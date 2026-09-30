#pragma once

#include "base/basic_types.h"

#include <gsl/pointers>

#include <memory>

namespace Main {
class Session;
} // namespace Main

namespace Ui {
class TranslateProvider;
} // namespace Ui

namespace Serein {

[[nodiscard]] std::unique_ptr<Ui::TranslateProvider> CreateInteractiveTranslateProvider(
	gsl::not_null<Main::Session*> session,
	Fn<void(QString)> error);

} // namespace Serein
