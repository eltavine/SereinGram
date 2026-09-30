#pragma once

#include <gsl/pointers>

#include <span>
#include <string_view>

namespace Main {
class Session;
} // namespace Main

namespace Serein::App {

struct Module {
	std::string_view id;
	void (*started)() = nullptr;
	void (*sessionStarted)(gsl::not_null<Main::Session*> session) = nullptr;
};

[[nodiscard]] std::span<const Module> Modules();

} // namespace Serein::App
