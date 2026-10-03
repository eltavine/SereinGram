#pragma once

#include "base/assertion.h"
#include "ui/text/text_entity.h"

namespace Serein::Messages {

// CommonMark with GitHub extensions; spoilers keep Telegram's || marks.
[[nodiscard]] QString ToMarkdown(const TextWithEntities &text);

} // namespace Serein::Messages
