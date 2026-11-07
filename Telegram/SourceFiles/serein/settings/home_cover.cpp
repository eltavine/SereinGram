#include "serein/settings/home_cover.h"

#include "serein/core/build_info.h"
#include "serein/features/updates/checker.h"
#include "boxes/about_box.h"
#include "lang/lang_keys.h"
#include "settings/settings_builder.h"
#include "ui/click_handler.h"
#include "ui/painter.h"
#include "ui/rp_widget.h"
#include "ui/ui_utility.h"
#include "ui/widgets/labels.h"
#include "ui/wrap/vertical_layout.h"
#include "window/main_window.h"

#include "styles/style_serein.h"
#include "styles/style_settings.h"

#include <string_view>

namespace Serein {
namespace {

using Updates::Status;

[[nodiscard]] QString VersionText() {
	auto result = tr::lng_settings_current_version(
		tr::now,
		lt_version,
		currentVersionShortText());
	const auto commit = std::string_view(kBuildCommit);
	if (std::string_view(kBuildChannel) == "nightly" && commit.size() == 40) {
		result += u" \u00B7 "_q + tr::lng_serein_cover_nightly(
			tr::now,
			lt_commit,
			QString::fromLatin1(commit.data(), 7));
	}
	return result;
}

[[nodiscard]] TextWithEntities StatusText(const Updates::UpdateState &state) {
	const auto check = [](QString text) {
		return TextWithEntities{ text + u" \u00B7 "_q }.append(
			tr::link(tr::lng_serein_update_check_now(tr::now)));
	};
	switch (state.status) {
	case Status::Checking:
		return { tr::lng_serein_update_checking(tr::now) };
	case Status::UpToDate:
		return { tr::lng_serein_cover_up_to_date(tr::now) };
	case Status::Failed:
		return check(tr::lng_serein_cover_failed(tr::now));
	case Status::Managed:
		return { tr::lng_serein_cover_package(tr::now) };
	case Status::Available: {
		if (!state.update) {
			break;
		}
		const auto &update = *state.update;
		const auto target = update.download.isEmpty()
			? update.page
			: update.download;
		return TextWithEntities{ tr::lng_serein_cover_available(
			tr::now,
			lt_version,
			update.version) + u" \u00B7 "_q }.append(tr::link(
				tr::lng_serein_update_download_link(tr::now),
				target));
	}
	case Status::Unknown:
		break;
	}
	return tr::link(tr::lng_serein_update_check_now(tr::now));
}

} // namespace

object_ptr<Ui::RpWidget> CreateAppCover(QWidget *parent) {
	auto result = object_ptr<Ui::RpWidget>(parent);
	const auto cover = result.data();
	const auto logo = Ui::CreateChild<Ui::RpWidget>(cover);
	logo->resize(st::sereinHomeCoverLogo, st::sereinHomeCoverLogo);
	logo->paintRequest() | rpl::on_next([=] {
		auto p = QPainter(logo);
		auto hq = PainterHighQualityEnabler(p);
		p.drawImage(logo->rect(), Window::LogoNoMargin());
	}, logo->lifetime());
	const auto title = Ui::CreateChild<Ui::FlatLabel>(
		cover,
		tr::lng_serein_settings(),
		st::settingsCoverName);
	const auto version = Ui::CreateChild<Ui::FlatLabel>(
		cover,
		VersionText(),
		st::settingsCoverStatus);
	const auto status = Ui::CreateChild<Ui::FlatLabel>(
		cover,
		st::settingsCoverStatus);
	Updates::StateValue(
	) | rpl::on_next([=](const Updates::UpdateState &state) {
		status->setMarkedText(StatusText(state));
		if (state.status == Status::Unknown
			|| state.status == Status::Failed) {
			status->setLink(1, std::make_shared<LambdaClickHandler>([] {
				Updates::CheckForUpdatesNow(nullptr);
			}));
		}
	}, status->lifetime());

	const auto &padding = st::sereinHomeCoverPadding;
	rpl::combine(
		cover->widthValue(),
		status->naturalWidthValue()
	) | rpl::on_next([=](int width, int) {
		const auto available = width - padding.left() - padding.right();
		const auto place = [&](not_null<Ui::RpWidget*> widget, int top) {
			widget->moveToLeft((width - widget->width()) / 2, top, width);
			return top + widget->height();
		};
		const auto fit = [&](not_null<Ui::FlatLabel*> label, int top) {
			label->resizeToNaturalWidth(available);
			return place(label, top);
		};
		auto top = place(logo, padding.top()) + st::sereinHomeCoverLogoSkip;
		top = fit(title, top) + st::sereinHomeCoverLineSkip;
		top = fit(version, top) + st::sereinHomeCoverLineSkip;
		top = fit(status, top);
		cover->resize(width, top + padding.bottom());
	}, cover->lifetime());
	return result;
}

void AddHomeCover(::Settings::Builder::SectionBuilder &builder) {
	builder.add([](const ::Settings::Builder::WidgetContext &context) {
		return ::Settings::Builder::SectionBuilder::WidgetToAdd{
			.widget = CreateAppCover(context.container),
		};
	});
}

} // namespace Serein
