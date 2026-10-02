#include "serein/hooks/interface/text.h"

#include "serein/interface/options.h"

namespace Serein::Interface {
namespace {

bool &HalfwidthEnabled() {
	static auto enabled = false;
	return enabled;
}

} // namespace

QString UiText(QString value) {
	return HalfwidthEnabled()
		? HalfwidthPunctuation(std::move(value))
		: value;
}

void StartUiText() {
	HalfwidthEnabled() = ForDevice().Get(kHalfwidthUiPunctuation);
}

} // namespace Serein::Interface
