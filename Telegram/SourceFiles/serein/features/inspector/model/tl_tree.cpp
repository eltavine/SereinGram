#include "serein/features/inspector/model/tl_tree.h"

#include "base/basic_types.h"

namespace Serein::Inspector {
namespace {

constexpr auto kMaxDepth = 64;

class Parser final {
public:
	explicit Parser(QStringView text) : _text(text) {
	}

	[[nodiscard]] std::optional<Node> parse() {
		auto result = Node();
		if (!value(result, 0)) {
			return std::nullopt;
		}
		return result;
	}

private:
	[[nodiscard]] bool at(QStringView token) const {
		return _text.mid(_position).startsWith(token);
	}
	bool skip(QStringView token) {
		if (!at(token)) {
			return false;
		}
		_position += token.size();
		return true;
	}
	void skipSpaces() {
		while (_position < _text.size() && _text[_position].isSpace()) {
			++_position;
		}
	}
	[[nodiscard]] bool finished() const {
		return _position >= _text.size();
	}
	[[nodiscard]] qsizetype endOf(QStringView stop) const {
		const auto index = _text.indexOf(stop, _position);
		return (index < 0) ? _text.size() : index;
	}
	void skipUntil(QStringView stop) {
		_position = endOf(stop);
	}
	[[nodiscard]] QString until(QStringView stop) {
		const auto end = endOf(stop);
		const auto result = _text.mid(_position, end - _position).toString();
		_position = end;
		return result;
	}
	[[nodiscard]] QString word() {
		const auto start = _position;
		while (!finished()) {
			const auto ch = _text[_position];
			if (ch.isSpace() || ch == u'}' || ch == u',' || ch == u':') {
				break;
			}
			++_position;
		}
		return _text.mid(start, _position - start).toString();
	}

	[[nodiscard]] bool value(Node &node, int depth);
	[[nodiscard]] bool object(Node &node, int depth);
	[[nodiscard]] bool vector(Node &node, int depth);
	[[nodiscard]] bool string(Node &node);
	[[nodiscard]] bool scalar(Node &node);
	[[nodiscard]] bool fields(Node &node, int depth, bool named);

	QStringView _text;
	qsizetype _position = 0;

};

bool Parser::value(Node &node, int depth) {
	if (depth > kMaxDepth) {
		return false;
	}
	skipSpaces();
	if (skip(u"[GZIPPED] ")) {
		return value(node, depth + 1);
	} else if (skip(u"[LAYER")) {
		skipUntil(u"] ");
		return skip(u"] ") && value(node, depth + 1);
	} else if (at(u"{ ")) {
		return object(node, depth);
	} else if (at(u"[ vector<")) {
		return vector(node, depth);
	} else if (at(u"\"")) {
		return string(node);
	} else if (skip(u"YES [ BY BIT ")) {
		node.kind = NodeKind::Flag;
		node.value = u"true"_q;
		skipUntil(u"]");
		return skip(u"]");
	} else if (at(u"[ERROR]")) {
		return false;
	}
	return scalar(node);
}

bool Parser::object(Node &node, int depth) {
	skip(u"{ ");
	node.kind = NodeKind::Object;
	node.type = word();
	return skip(u" }") || fields(node, depth, true);
}

bool Parser::vector(Node &node, int depth) {
	skip(u"[ vector<");
	node.kind = NodeKind::Vector;
	node.type = until(u"> (");
	if (!skip(u"> (")) {
		return false;
	}
	skipUntil(u")");
	return skip(u")") && fields(node, depth, false);
}

bool Parser::fields(Node &node, int depth, bool named) {
	const auto close = named ? QStringView(u"}") : QStringView(u"]");
	while (true) {
		skipSpaces();
		if (skip(close)) {
			return true;
		} else if (finished()) {
			return false;
		}
		auto child = Node();
		if (named) {
			child.key = word();
			if (child.key.isEmpty() || !skip(u": ")) {
				return false;
			}
		} else {
			child.key = QString::number(node.children.size());
		}
		if (!value(child, depth + 1)) {
			return false;
		}
		node.children.push_back(std::move(child));
		skipSpaces();
		skip(u",");
	}
}

bool Parser::string(Node &node) {
	skip(u"\"");
	node.kind = NodeKind::String;
	auto result = QString();
	while (!finished()) {
		const auto ch = _text[_position++];
		if (ch == u'"') {
			node.value = std::move(result);
			return skip(u" [STRING]");
		} else if (ch == u'\\' && !finished()) {
			const auto next = _text[_position++];
			result.append((next == u'n') ? QChar(u'\n') : next);
		} else {
			result.append(ch);
		}
	}
	return false;
}

bool Parser::scalar(Node &node) {
	const auto text = until(u" [");
	if (!skip(u" [")) {
		return false;
	}
	const auto tag = until(u"]");
	if (!skip(u"]")) {
		return false;
	}
	node.kind = tag.endsWith(u" BYTES") ? NodeKind::Bytes : NodeKind::Number;
	node.type = tag;
	node.value = text.trimmed();
	return !node.value.isEmpty();
}

} // namespace

const Node *Node::find(QStringView name) const {
	for (const auto &child : children) {
		if (child.key == name) {
			return &child;
		}
	}
	return nullptr;
}

std::optional<Node> ParseDump(QStringView text) {
	auto result = Parser(text).parse();
	if (!result || result->type != u"core_message"_q) {
		return result;
	}
	const auto body = result->find(u"body");
	if (!body) {
		return std::nullopt;
	}
	auto unwrapped = *body;
	unwrapped.key = QString();
	return unwrapped;
}

} // namespace Serein::Inspector
