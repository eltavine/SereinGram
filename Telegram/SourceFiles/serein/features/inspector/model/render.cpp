#include "serein/features/inspector/model/render.h"

#include "base/basic_types.h"

#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

namespace Serein::Inspector {
namespace {

[[nodiscard]] QString Quoted(QString value) {
	value.replace(u'\\', u"\\\\"_q);
	value.replace(u'"', u"\\\""_q);
	value.replace(u'\n', u"\\n"_q);
	return QChar(u'"') + value + QChar(u'"');
}

[[nodiscard]] QString Shown(const Node &node) {
	switch (node.kind) {
	case NodeKind::Object:
		return node.type;
	case NodeKind::Vector:
		return u"["_q + QString::number(node.children.size()) + u"]"_q;
	case NodeKind::String:
		return Quoted(node.value);
	case NodeKind::Bytes:
		return node.value + u" ("_q + node.type.toLower() + u")"_q;
	case NodeKind::Flag:
	case NodeKind::Number:
		break;
	}
	return node.value;
}

void RenderInto(const Node &node, int depth, QString &out) {
	out.append(QString(qsizetype(depth) * 2, QChar(u' ')));
	if (!node.key.isEmpty()) {
		out.append(node.key + u": "_q);
	}
	out.append(Shown(node));
	out.append(QChar(u'\n'));
	for (const auto &child : node.children) {
		RenderInto(child, depth + 1, out);
	}
}

[[nodiscard]] QJsonValue ToJson(const Node &node) {
	switch (node.kind) {
	case NodeKind::Object: {
		auto object = QJsonObject();
		object.insert(u"_"_q, node.type);
		for (const auto &child : node.children) {
			object.insert(child.key, ToJson(child));
		}
		return object;
	}
	case NodeKind::Vector: {
		auto array = QJsonArray();
		for (const auto &child : node.children) {
			array.push_back(ToJson(child));
		}
		return array;
	}
	case NodeKind::Flag:
		return true;
	case NodeKind::Number:
		if (node.type == u"INT"_q || node.type == u"DOUBLE"_q) {
			auto ok = false;
			const auto value = node.value.toDouble(&ok);
			if (ok) {
				return value;
			}
		}
		break;
	case NodeKind::String:
	case NodeKind::Bytes:
		break;
	}
	return node.value;
}

} // namespace

QString RenderText(const Node &root) {
	auto result = QString();
	RenderInto(root, 0, result);
	if (result.endsWith(QChar(u'\n'))) {
		result.chop(1);
	}
	return result;
}

QByteArray RenderJson(const Node &root) {
	const auto value = ToJson(root);
	const auto document = value.isArray()
		? QJsonDocument(value.toArray())
		: value.isObject()
		? QJsonDocument(value.toObject())
		: QJsonDocument(QJsonObject{ { u"value"_q, value } });
	return document.toJson(QJsonDocument::Indented);
}

QString Humanize(const QString &name) {
	auto result = QString(name).replace(u'_', u' ');
	if (!result.isEmpty()) {
		result[0] = result[0].toUpper();
	}
	return result;
}

} // namespace Serein::Inspector
