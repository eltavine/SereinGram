#include "serein/services/translation_context.h"

#include "serein/hooks/privacy/alias.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "history/history_item.h"
#include "history/view/history_view_element.h"
#include "main/main_session.h"

namespace Serein {
namespace {

constexpr auto kContextMessages = 6;
constexpr auto kContextLineLimit = 300;

} // namespace

QStringList TranslationContext(
		not_null<Main::Session*> session,
		uint64 peerId,
		int64 msgId) {
	if (!msgId) {
		return {};
	}
	const auto item = session->data().message(PeerId(peerId), MsgId(msgId));
	auto element = item ? item->mainView() : nullptr;
	auto result = QStringList();
	while (element && result.size() < kContextMessages) {
		element = element->previousInBlocks();
		const auto data = element ? element->data().get() : nullptr;
		const auto text = data
			? data->originalText().text.trimmed()
			: QString();
		if (data && !data->isService() && !text.isEmpty()) {
			result.push_front(Privacy::DisplayName(data->from())
				+ u": "_q
				+ text.left(kContextLineLimit));
		}
	}
	return result;
}

} // namespace Serein
