#include "serein/compose/options.h"

#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

namespace Serein::Compose {

bool ValidQuickReplies(const QByteArray &value) {
	const auto json = QJsonDocument::fromJson(value);
	if (!json.isObject()) {
		return false;
	}
	const auto object = json.object();
	const auto replies = object.value(QString::fromLatin1("replies")).toArray();
	return object.size() == 2
		&& object.value(QString::fromLatin1("version")) == 1
		&& replies.size() == 2
		&& replies[0].isString()
		&& replies[1].isString();
}

} // namespace Serein::Compose
