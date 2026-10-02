#include "serein/hooks/privacy/sessions.h"

#include "serein/privacy/options.h"
#include "api/api_authorizations.h"
#include "base/unixtime.h"
#include "lang/lang_keys.h"
#include "settings/sections/settings_active_sessions.h"
#include "ui/wrap/vertical_layout.h"
#include "styles/style_menu_icons.h"

namespace Serein::Privacy {

template <typename Entry>
void AddSessionDetails(
		not_null<Ui::VerticalLayout*> container,
		const Entry &entry) {
	if (!ForDevice().Get(kShowSessionDetails)) {
		return;
	}
	if (entry.createdTime) {
		Settings::AddSessionInfoRow(
			container,
			tr::lng_serein_session_created(),
			langDateTimeFull(base::unixtime::parse(entry.createdTime)),
			st::menuIconSchedule);
	}
	if (entry.apiId) {
		Settings::AddSessionInfoRow(
			container,
			tr::lng_serein_session_api_id(),
			QString::number(entry.apiId),
			st::menuIconBotCommands);
	}
	Settings::AddSessionInfoRow(
		container,
		tr::lng_serein_session_official(),
		(entry.officialApp ? tr::lng_box_yes : tr::lng_box_no)(tr::now),
		st::menuIconPermissions);
}

template void AddSessionDetails(
	not_null<Ui::VerticalLayout*> container,
	const Api::Authorizations::Entry &entry);

} // namespace Serein::Privacy
