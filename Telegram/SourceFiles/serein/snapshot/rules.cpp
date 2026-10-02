#include "serein/snapshot/rules.h"

#include "base/basic_types.h"
#include "serein/schema/gen/settings/snapshot.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

namespace Serein::Snapshot {

SnapshotConfig Defaults() {
	return {
		.background = true,
		.date = true,
		.headers = true,
		.reactions = true,
		.builtinTheme = false,
		.simpleReplies = false,
	};
}

std::optional<SnapshotConfig> ReadStored(const QByteArray &raw) {
	if (raw.isEmpty()) {
		return Defaults();
	} else if (auto result = ParseSnapshotConfig(raw)) {
		return result;
	}
	auto object = QJsonDocument::fromJson(raw).object();
	if (object.value(u"version"_q) != QJsonValue(1)
		|| object.contains(u"simpleReplies"_q)) {
		return std::nullopt;
	}
	object.insert(u"version"_q, 2);
	object.insert(u"simpleReplies"_q, false);
	return ParseSnapshotConfig(
		QJsonDocument(object).toJson(QJsonDocument::Compact));
}

bool Validate(const QByteArray &raw) {
	return ReadStored(raw).has_value();
}

} // namespace Serein::Snapshot
