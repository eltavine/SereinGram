#include "serein/features/history/chat_row.h"

#include "serein/core/options.h"
#include "serein/features/history/model/recorder.h"
#include "data/data_peer.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "ui/widgets/buttons.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"

#include "styles/style_settings.h"

namespace Serein::HistoryFeature {

void AddRecordingRow(const ChatSettingsContext &context) {
	const auto session = &context.controller->session();
	const auto policy = Read(ForAccount(session));
	if (!policy.saveDeleted && !policy.saveEdits) {
		return;
	}
	const auto peerId = qint64(context.peer->id.value);
	const auto button = context.container->add(
		object_ptr<Ui::SettingsButton>(
			context.container,
			tr::lng_serein_chat_record_history(),
			st::settingsButtonNoIcon));
	button->toggleOn(rpl::single(!Excluded(ForAccount(session), peerId)));
	button->toggledChanges(
	) | rpl::on_next([=](bool recorded) {
		Expects(SetExcluded(ForAccount(session), peerId, !recorded));
	}, button->lifetime());
}

} // namespace Serein::HistoryFeature
