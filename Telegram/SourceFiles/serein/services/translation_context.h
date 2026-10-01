#pragma once

#include <QtCore/QStringList>

namespace Main {
class Session;
} // namespace Main

namespace Serein {

[[nodiscard]] QStringList TranslationContext(
	not_null<Main::Session*> session,
	uint64 peerId,
	int64 msgId);

} // namespace Serein
