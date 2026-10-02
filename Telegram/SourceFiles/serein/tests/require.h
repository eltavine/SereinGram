#pragma once

#include <doctest/doctest.h>

#include <source_location>
#include <string>

inline void Require(
		bool value,
		const char *message,
		std::source_location where = std::source_location::current()) {
	INFO((std::string(message)
		+ " at "
		+ where.file_name()
		+ ':'
		+ std::to_string(where.line())));
	REQUIRE(value);
}
