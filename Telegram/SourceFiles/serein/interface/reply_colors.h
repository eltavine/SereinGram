#pragma once

namespace Serein::Interface {

class ThemeReplyColorsScope final {
public:
	explicit ThemeReplyColorsScope(bool enabled);
	~ThemeReplyColorsScope();

	ThemeReplyColorsScope(const ThemeReplyColorsScope &) = delete;
	ThemeReplyColorsScope &operator=(const ThemeReplyColorsScope &) = delete;

private:
	bool _was = false;

};

} // namespace Serein::Interface
