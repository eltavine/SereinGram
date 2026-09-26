#include "nagram/menu/model.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

namespace Nagram::Menu {
namespace {

QByteArray Key(ActionId id) {
	return "E" + QByteArray::number(static_cast<int>(id)).rightJustified(2, '0');
}

bool KnownKey(const QString &key) {
	for (const auto &entry : kEntries) {
		if (key == QString::fromLatin1(Key(entry.id))) {
			return true;
		}
	}
	return false;
}

QJsonObject Parse(const QByteArray &raw) {
	return QJsonDocument::fromJson(raw).object();
}

} // namespace

bool ValidateConfig(const QByteArray &raw) {
	if (raw.isEmpty()) {
		return true;
	}
	auto error = QJsonParseError();
	const auto document = QJsonDocument::fromJson(raw, &error);
	if (error.error != QJsonParseError::NoError || !document.isObject()) {
		return false;
	}
	const auto root = document.object();
	if (root.size() != 2 || !root.value("version").isDouble()
		|| root.value("version").toInt() != 1
		|| !root.value("states").isObject()) {
		return false;
	}
	const auto states = root.value("states").toObject();
	for (auto it = states.begin(); it != states.end(); ++it) {
		if (!KnownKey(it.key()) || !it.value().isString()) {
			return false;
		}
		const auto value = it.value().toString();
		if (value != u"hide" && value != u"option") {
			return false;
		}
	}
	return true;
}

Visibility ReadVisibility(const QByteArray &raw, ActionId id) {
	if (raw.isEmpty() || !ValidateConfig(raw)) {
		return Visibility::Show;
	}
	const auto value = Parse(raw).value("states").toObject().value(
		QString::fromLatin1(Key(id))).toString();
	return (value == u"hide")
		? Visibility::Hide
		: (value == u"option")
		? Visibility::WithOption
		: Visibility::Show;
}

QByteArray WriteVisibility(
		const QByteArray &raw,
		ActionId id,
		Visibility visibility) {
	Expects(ValidateConfig(raw));
	auto states = raw.isEmpty()
		? QJsonObject()
		: Parse(raw).value("states").toObject();
	const auto key = QString::fromLatin1(Key(id));
	if (visibility == Visibility::Show) {
		states.remove(key);
	} else {
		states.insert(key, visibility == Visibility::Hide
			? QString(u"hide") : QString(u"option"));
	}
	if (states.isEmpty()) {
		return QByteArray();
	}
	auto root = QJsonObject();
	root.insert(u"version", 1);
	root.insert(u"states", states);
	return QJsonDocument(root).toJson(QJsonDocument::Compact);
}

bool Visible(Visibility visibility, bool optionHeld) {
	return visibility == Visibility::Show
		|| (visibility == Visibility::WithOption && optionHeld);
}

} // namespace Nagram::Menu
