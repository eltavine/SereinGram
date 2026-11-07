#pragma once

#include <QtCore/QString>

#include <optional>
#include <vector>

namespace Serein::Inspector {

enum class NodeKind {
	Object,
	Vector,
	String,
	Bytes,
	Number,
	Flag,
};

struct Node {
	QString key;
	NodeKind kind = NodeKind::Object;
	QString type;
	QString value;
	std::vector<Node> children;

	[[nodiscard]] const Node *find(QStringView name) const;
};

// Reads MTP::details::DumpToText output and unwraps its core_message.
[[nodiscard]] std::optional<Node> ParseDump(QStringView text);

} // namespace Serein::Inspector
