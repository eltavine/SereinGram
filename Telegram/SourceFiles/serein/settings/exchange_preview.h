#pragma once

#include "serein/core/exchange.h"
#include "lang/lang_keys.h"

namespace Ui {
class GenericBox;
} // namespace Ui

namespace Window {
class SessionController;
} // namespace Window

namespace Serein {

struct ExchangePreviewTexts {
	tr::phrase<> title;
	tr::phrase<> about;
	tr::phrase<> apply;
	tr::phrase<> done;
};

[[nodiscard]] QString LocalizedKey(std::string_view key);
[[nodiscard]] QString OptionTitle(const OptionInfo &info);
[[nodiscard]] QString ScopedOptionTitle(const OptionInfo &info);
[[nodiscard]] Options *AccountOptions(
	not_null<Window::SessionController*> controller);

void AddSelectableText(not_null<Ui::GenericBox*> box, const QString &text);
void ShowExchangePreview(
	not_null<Window::SessionController*> controller,
	const ExchangePlan &plan,
	const ExchangePreviewTexts &texts);

} // namespace Serein
