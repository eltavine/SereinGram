#include "serein/menu/model.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

namespace Serein::Menu {
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

Visibility DefaultVisibility(ActionId id) {
	if (id == ActionId::EditHistory || id == ActionId::DeletedMessages) {
		return Visibility::Show;
	}
	return static_cast<int>(id) >= static_cast<int>(ActionId::Repeat)
		? Visibility::Hide : Visibility::Show;
}

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
	const auto version = root.value("version").toInt();
	if (root.size() != 2 || !root.value("version").isDouble()
		|| (version != 1 && version != 2)
		|| !root.value("states").isObject()) {
		return false;
	}
	const auto states = root.value("states").toObject();
	for (auto it = states.begin(); it != states.end(); ++it) {
		if (!KnownKey(it.key()) || (version == 1 && it.key() == u"E22")
			|| !it.value().isString()) {
			return false;
		}
		const auto value = it.value().toString();
		if (value != u"show" && value != u"hide"
			&& value != u"option") {
			return false;
		}
	}
	return true;
}

Visibility ReadVisibility(const QByteArray &raw, ActionId id) {
	if (raw.isEmpty() || !ValidateConfig(raw)) {
		return DefaultVisibility(id);
	}
	const auto root = Parse(raw);
	if (root.value("version").toInt() == 1) {
		if (id == ActionId::Screenshot) {
			return DefaultVisibility(id);
		}
		if (id == ActionId::Reading) {
			id = ActionId::Screenshot;
		}
	}
	const auto value = root.value("states").toObject().value(
		QString::fromLatin1(Key(id))).toString();
	return (value == u"hide")
		? Visibility::Hide
		: (value == u"option")
		? Visibility::WithOption
		: value == u"show"
		? Visibility::Show
		: DefaultVisibility(id);
}

QByteArray WriteVisibility(
		const QByteArray &raw,
		ActionId id,
		Visibility visibility) {
	Expects(ValidateConfig(raw));
	auto states = raw.isEmpty()
		? QJsonObject()
		: Parse(raw).value("states").toObject();
	if (!raw.isEmpty() && Parse(raw).value("version").toInt() == 1) {
		const auto oldReading = states.take(u"E21");
		if (!oldReading.isUndefined()) {
			states.insert(u"E22", oldReading);
		}
	}
	const auto key = QString::fromLatin1(Key(id));
	if (visibility == DefaultVisibility(id)) {
		states.remove(key);
	} else {
		states.insert(key, visibility == Visibility::Hide
			? QString(u"hide")
			: visibility == Visibility::WithOption
			? QString(u"option") : QString(u"show"));
	}
	if (states.isEmpty()) {
		return QByteArray();
	}
	auto root = QJsonObject();
	root.insert(u"version", 2);
	root.insert(u"states", states);
	return QJsonDocument(root).toJson(QJsonDocument::Compact);
}

bool Visible(Visibility visibility, bool optionHeld) {
	return visibility == Visibility::Show
		|| (visibility == Visibility::WithOption && optionHeld);
}

} // namespace Serein::Menu
