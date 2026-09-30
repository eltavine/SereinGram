#include <iostream>

namespace base::assertion {

void log(const char *message, const char *file, int line) {
	std::cerr << message << " (" << file << ":" << line << ")" << std::endl;
}

} // namespace base::assertion
