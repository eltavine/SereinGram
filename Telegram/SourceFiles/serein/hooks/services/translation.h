#pragma once

#include "base/basic_types.h"

#include <gsl/pointers>
#include <rpl/producer.h>

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
[[nodiscard]] std::unique_ptr<Ui::TranslateProvider> CreateChatTranslateProvider(
	gsl::not_null<Main::Session*> session);
[[nodiscard]] rpl::producer<bool> ChatTranslationAllowedValue(
	gsl::not_null<Main::Session*> session,
	gsl::not_null<Ui::TranslateProvider*> provider);
[[nodiscard]] bool ChatTranslationAllowed(gsl::not_null<Main::Session*> session);

} // namespace Serein
