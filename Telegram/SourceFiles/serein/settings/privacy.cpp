#include "serein/settings/privacy.h"

#include "serein/admin/unblock_all.h"
#include "serein/privacy/options.h"
#include "serein/privacy/qr_scan.h"
#include "serein/settings/gen/ghost_rows.h"
#include "serein/settings/gen/history_rows.h"
#include "serein/settings/gen/privacy_rows.h"
#include "serein/settings/home.h"
#include "serein/settings/subpages.h"
#include "serein/settings/page.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "main/main_session_settings.h"
#include "settings/settings_builder.h"
#include "settings/settings_common_session.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/checkbox.h"
#include "ui/layers/generic_box.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"
#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"

namespace Serein {
namespace {

using namespace ::Settings;
using namespace ::Settings::Builder;

class PrivacySection final : public Page<PrivacySection> {
public:
	using Page::Page;

	[[nodiscard]] rpl::producer<QString> title() override {
		return tr::lng_serein_privacy();
	}

	static const SectionBuildMethod kBuild;

};

const auto kMeta = BuildHelper({
	.id = PrivacySection::Id(),
	.parentId = HomeId(),
	.title = Privacy::kSubpageTitle,
	.icon = Privacy::kSubpageIcon,
}, [](SectionBuilder &builder) {
	builder.addSkip();
	Ghost::AddSubpageButton(builder, GhostId());
	HistorySettings::AddSubpageButton(builder, HistoryId());
	EndSection(builder);
	Privacy::AddLayout(builder);
	AddSection(builder, {
		u"serein/privacy/account-tools"_q,
		tr::lng_serein_section_account_tools,
		{ u"phone"_q, u"QR"_q, u"blocked"_q },
	});
	const auto session = builder.session();
	const auto button = builder.addButton({
		.id = u"serein/privacy/hide-my-phone"_q,
		.title = tr::lng_serein_hide_my_phone(),
		.st = &st::settingsButtonNoIcon,
		.toggled = session->settings().phoneNumberHiddenValue(),
		.keywords = { u"phone"_q, u"number"_q },
	});
	if (button) {
		button->toggledChanges(
		) | rpl::on_next([=](bool value) {
			session->settings().setPhoneNumberHidden(value);
			session->saveSettingsDelayed();
		}, button->lifetime());
	}
	const auto controller = builder.controller();
	builder.addButton({
		.id = u"serein/privacy/unblock-all"_q,
		.title = tr::lng_serein_unblock_all(),
		.st = &st::settingsAttentionButton,
		.onClick = [=] {
			if (controller) {
				Admin::ConfirmUnblockAll(controller);
			}
		},
		.keywords = { u"blocked"_q, u"unblock"_q },
	});
	builder.addButton({
		.id = u"serein/privacy/qr-scan"_q,
		.title = tr::lng_serein_qr_scan(),
		.st = &st::settingsButtonNoIcon,
		.onClick = [=] {
			if (controller) {
				Privacy::ShowQrScanner(controller);
			}
		},
		.keywords = { u"QR"_q, u"login"_q, u"scan"_q },
	});
	EndSection(builder, tr::lng_serein_qr_scan_about);
});

const SectionBuildMethod PrivacySection::kBuild = kMeta.build;

} // namespace

Settings::Type PrivacyId() {
	return PrivacySection::Id();
}

} // namespace Serein
