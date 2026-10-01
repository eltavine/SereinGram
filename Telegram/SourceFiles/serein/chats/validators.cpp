#include "serein/chats/options.h"

namespace Serein::Chats {

bool ValidChatSort(const int &value) {
	if (value == 0) {
		return true;
	} else if (value < 0 || value > 0xFFF) {
		return false;
	}
	auto seen = 0;
	for (auto i = 0; i != 4; ++i) {
		seen |= 1 << ((value >> (4 + i * 2)) & 3);
	}
	return seen == 0xF;
}

bool ValidManagedFolderIds(const QString &value) {
	if (value.isEmpty()) {
		return true;
	}
	auto previous = 0;
	for (const auto &part : value.split(u',')) {
		auto valid = false;
		const auto id = part.toInt(&valid);
		if (!valid || id <= previous || part != QString::number(id)) {
			return false;
		}
		previous = id;
	}
	return true;
}

bool ValidHiddenFolderIds(const QString &value) {
	return ValidManagedFolderIds(value);
}

} // namespace Serein::Chats
