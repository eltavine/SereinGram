#pragma once

#include <QtCore/QString>
#include <rpl/producer.h>

#include <vector>

namespace Shortcuts {
enum class Command;
} // namespace Shortcuts

namespace Serein::Hooks {

struct ShortcutEntry {
	Shortcuts::Command command;
	rpl::producer<QString> label;
};

[[nodiscard]] std::vector<ShortcutEntry> ShortcutEntries();

template <typename Entry>
[[nodiscard]] std::vector<Entry> WithShortcutEntries(
		std::vector<Entry> entries) {
	auto added = ShortcutEntries();
	if (!added.empty()) {
		entries.push_back(Entry{ {}, nullptr });
	}
	for (auto &entry : added) {
		entries.push_back(Entry{ entry.command, std::move(entry.label) });
	}
	return entries;
}

} // namespace Serein::Hooks
