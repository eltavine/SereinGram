#include "serein/hooks/settings/about.h"

#include "serein/settings/home_cover.h"
#include "serein/settings/licenses.h"
#include "boxes/about_box.h"
#include "lang/lang_keys.h"
#include "ui/controls/feature_list.h"
#include "ui/layers/generic_box.h"
#include "ui/layers/show.h"
#include "ui/widgets/labels.h"
#include "ui/vertical_list.h"

#include "styles/style_boxes.h"
#include "styles/style_layers.h"
#include "styles/style_menu_icons.h"
#include "styles/style_serein.h"

#include <vector>

namespace Serein {
namespace {

using Entries = std::vector<Ui::FeatureListEntry>;

[[nodiscard]] QString Repository(const QString &path = QString()) {
	return u"https://github.com/eltavine/SereinGram"_q + path;
}

[[nodiscard]] Entries NoticeEntries() {
	return {
		{
			st::menuIconInfo,
			tr::lng_serein_about_unofficial_title(tr::now),
			tr::lng_serein_about_unofficial(
				tr::now,
				lt_api_link,
				tr::lng_serein_about_api_link(
					tr::now,
					tr::url(u"https://core.telegram.org/api"_q)),
				tr::marked),
		},
		{
			st::menuIconGroups,
			tr::lng_serein_about_references_title(tr::now),
			tr::lng_serein_about_references(tr::now, tr::marked),
		},
		{
			st::menuIconReport,
			tr::lng_serein_about_terms_title(tr::now),
			tr::lng_serein_about_terms(
				tr::now,
				lt_terms_link,
				tr::lng_serein_about_terms_link(
					tr::now,
					tr::url(u"https://telegram.org/tos"_q)),
				tr::marked),
		},
		{
			st::menuIconStealth,
			tr::lng_serein_about_respect_title(tr::now),
			tr::lng_serein_about_respect(tr::now, tr::marked),
		},
		{
			st::menuIconExperimental,
			tr::lng_serein_about_warranty_title(tr::now),
			tr::lng_serein_about_warranty(tr::now, tr::marked),
		},
		{
			st::menuIconCopyright,
			tr::lng_serein_about_names_title(tr::now),
			tr::lng_serein_about_names(tr::now, tr::marked),
		},
	};
}

[[nodiscard]] Entries PrivacyEntries() {
	return {
		{
			st::menuIconLock,
			tr::lng_serein_about_tracking_title(tr::now),
			tr::lng_serein_about_tracking(tr::now, tr::marked),
		},
		{
			st::menuIconTranslate,
			tr::lng_serein_about_services_title(tr::now),
			tr::lng_serein_about_services(tr::now, tr::marked),
		},
		{
			st::menuIconStorage,
			tr::lng_serein_about_local_title(tr::now),
			tr::lng_serein_about_local(tr::now, tr::marked),
		},
	};
}

[[nodiscard]] TextWithEntities LicenseText() {
	auto result = tr::lng_serein_about_license(
		tr::now,
		lt_license_link,
		tr::lng_serein_about_license_link(
			tr::now,
			tr::url(Repository(u"/blob/HEAD/LICENSE"_q))),
		lt_github_link,
		tr::link(u"GitHub"_q, Repository()),
		tr::marked);
	result.append(u"\n\n"_q);
	result.append(u"Copyright \u00A9 2014\u20132026 The Telegram Desktop Authors"_q);
	result.append(u"\nCopyright \u00A9 2026 The SereinGram Authors"_q);
	return result;
}

[[nodiscard]] Entries OpenSourceEntries() {
	return {
		{
			st::menuIconArticle,
			tr::lng_serein_about_license_title(tr::now),
			LicenseText(),
		},
		{
			st::menuIconFave,
			tr::lng_serein_about_credits_title(tr::now),
			tr::lng_serein_about_credits(
				tr::now,
				lt_designer_link,
				tr::link(u"OukaroMF"_q, u"https://github.com/OukaroMF/"_q),
				tr::marked),
		},
		{
			st::menuIconFaq,
			tr::lng_serein_about_help_title(tr::now),
			tr::lng_serein_about_help(
				tr::now,
				lt_releases_link,
				tr::link(u"GitHub Releases"_q, Repository(u"/releases"_q)),
				lt_issues_link,
				tr::link(u"GitHub Issues"_q, Repository(u"/issues"_q)),
				lt_faq_link,
				tr::lng_serein_about_faq_link(
					tr::now,
					tr::url(telegramFaqLink())),
				tr::marked),
		},
	};
}

void AddSection(
		not_null<Ui::GenericBox*> box,
		rpl::producer<QString> title,
		const Entries &entries) {
	const auto container = box->verticalLayout();
	Ui::AddDivider(container);
	Ui::AddSkip(container);
	Ui::AddSubsectionTitle(container, std::move(title));
	for (const auto &entry : entries) {
		box->addRow(Ui::MakeFeatureListEntry(box, entry));
	}
	Ui::AddSkip(container);
}

} // namespace

void AboutBox(not_null<Ui::GenericBox*> box) {
	box->setWidth(st::aboutWidth);
	box->setNoContentMargin(true);
	box->addTopButton(st::boxTitleClose, [=] {
		box->closeBox();
	});
	box->addRow(CreateAppCover(box), style::margins());
	box->addRow(
		object_ptr<Ui::FlatLabel>(
			box,
			tr::lng_serein_about_intro(),
			st::sereinAboutIntro),
		st::boxRowPadding,
		style::al_top
	)->setTryMakeSimilarLines(true);
	Ui::AddSkip(box->verticalLayout());
	AddSection(box, tr::lng_serein_about_notices(), NoticeEntries());
	AddSection(box, tr::lng_serein_about_privacy(), PrivacyEntries());
	AddSection(
		box,
		tr::lng_serein_about_open_source(),
		OpenSourceEntries());
	box->addLeftButton(tr::lng_serein_licenses(), [=] {
		box->uiShow()->showBox(Box(LicensesBox));
	});
	box->addButton(tr::lng_close(), [=] {
		box->closeBox();
	});
}

} // namespace Serein
