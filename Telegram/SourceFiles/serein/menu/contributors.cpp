#include "serein/menu/contributors.h"

namespace Serein::Menu {
namespace {

[[nodiscard]] std::vector<Contributor> &Contributors() {
	static auto result = std::vector<Contributor>();
	return result;
}

} // namespace

void AddContributor(Contributor contributor) {
	Contributors().push_back(std::move(contributor));
}

void Contribute(const Context &context) {
	for (const auto &contributor : Contributors()) {
		contributor(context);
	}
}

} // namespace Serein::Menu
