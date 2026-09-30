#include "serein/menu/model.h"

#include "serein/schema/gen/config/menu.h"
#include "base/basic_types.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include <algorithm>

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
	if (id == ActionId::EditHistory
		|| id == ActionId::DeletedMessages
		|| id == ActionId::ReadUntilHere
		|| id == ActionId::HistoryExclusion) {
		return Visibility::Show;
	}
	return static_cast<int>(id) >= static_cast<int>(ActionId::Repeat)
		? Visibility::Hide : Visibility::Show;
}

bool ValidMenuConfig(const MenuConfig &value) {
	return std::all_of(value.states.begin(), value.states.end(), [](
			const auto &entry) {
		return KnownKey(entry.first);
	});
}

bool ValidateConfig(const QByteArray &raw) {
	if (raw.isEmpty()) {
		return true;
	}
	auto root = QJsonDocument::fromJson(raw).object();
	if (root.value(u"version"_q) != QJsonValue(1)) {
		return ParseMenuConfig(raw).has_value();
	} else if (root.value(u"states"_q).toObject().contains(u"E22"_q)) {
		return false;
	}
	root.insert(u"version"_q, 2);
	return ParseMenuConfig(
		QJsonDocument(root).toJson(QJsonDocument::Compact)).has_value();
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
