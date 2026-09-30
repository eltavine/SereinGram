#include "serein/compose/options.h"
#include "serein/schema/gen/config/quick_replies.h"

namespace Serein::Compose {

bool ValidQuickReplies(const QByteArray &value) {
	return ParseQuickReplies(value).has_value();
}

} // namespace Serein::Compose
