#include "serein/hooks/interface/reply_colors.h"
#include "serein/interface/reply_colors.h"

#include "serein/hooks/gen/interface.h"

#include <utility>

namespace Serein {
namespace {

bool Forced = false;

} // namespace

bool Hooks::Interface::UseThemeReplyColors() {
	return Forced || ThemeReplyColors();
}

Interface::ThemeReplyColorsScope::ThemeReplyColorsScope(bool enabled)
: _was(std::exchange(Forced, Forced || enabled)) {
}

Interface::ThemeReplyColorsScope::~ThemeReplyColorsScope() {
	Forced = _was;
}

} // namespace Serein
