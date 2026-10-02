#include "serein/features/stories/composer.h"

#include "serein/features/stories/audience.h"
#include "serein/features/stories/canvas.h"
#include "serein/features/stories/publisher.h"
#include "apiwrap.h"
#include "chat_helpers/compose/compose_show.h"
#include "chat_helpers/message_field.h"
#include "core/application.h"
#include "core/file_utilities.h"
#include "data/data_peer.h"
#include "lang/lang_keys.h"
#include "main/main_app_config.h"
#include "main/main_session.h"
#include "mtproto/mtproto_response.h"
#include "storage/storage_media_prepare.h"
#include "ui/chat/attach/attach_prepare.h"
#include "ui/layers/generic_box.h"
#include "ui/painter.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/checkbox.h"
#include "ui/widgets/fields/input_field.h"
#include "ui/widgets/labels.h"
#include "ui/wrap/slide_wrap.h"
#include "window/window_session_controller.h"
#include "styles/style_basic.h"
#include "styles/style_boxes.h"
#include "styles/style_layers.h"
#include "styles/style_serein.h"
#include "styles/style_settings.h"

#include <QtGui/QPainterPath>

#include <cmath>

namespace Serein::Stories {
namespace {

constexpr auto kHour = 3600;
constexpr auto kCaptionLimit = 200;
constexpr auto kPremiumCaptionLimit = 2048;

struct ComposerState {
	explicit ComposerState(not_null<Main::Session*> session)
	: publisher(session) {
	}

	std::optional<Ui::PreparedFile> file;
	QImage preview;
	rpl::variable<AudienceValue> audience;
	int period = kDefaultPeriod;
	bool posting = false;
	Publisher publisher;
};

[[nodiscard]] QString ErrorText(const QString &type) {
	switch (ClassifyError(type)) {
	case PostError::TooMany:
		return tr::lng_serein_story_error_limit(tr::now);
	case PostError::PremiumRequired:
		return tr::lng_serein_story_error_premium(tr::now);
	case PostError::BoostsRequired:
		return tr::lng_serein_story_error_boosts(tr::now);
	case PostError::WeeklyLimit:
		return tr::lng_serein_story_error_weekly(tr::now);
	case PostError::MonthlyLimit:
		return tr::lng_serein_story_error_monthly(tr::now);
	case PostError::Other:
		break;
	}
	return type.isEmpty() ? tr::lng_attach_failed(tr::now) : type;
}

[[nodiscard]] QString StageText(Stage stage, float64 value) {
	switch (stage) {
	case Stage::Preparing:
		return tr::lng_serein_story_preparing(tr::now);
	case Stage::Uploading:
		return tr::lng_serein_story_uploading(
			tr::now,
			lt_percent,
			QString::number(int(std::round(value * 100))));
	case Stage::Publishing:
		return tr::lng_serein_story_publishing(tr::now);
	}
	Unexpected("Stage in Serein::Stories::StageText.");
}

[[nodiscard]] int CaptionLimit(not_null<Main::Session*> session) {
	const auto &config = session->appConfig();
	return session->premium()
		? config.get<int>(
			u"story_caption_length_limit_premium"_q,
			kPremiumCaptionLimit)
		: config.get<int>(
			u"story_caption_length_limit_default"_q,
			kCaptionLimit);
}

[[nodiscard]] bool TooLong(const Ui::PreparedFile &file) {
	using Video = Ui::PreparedFileInformation::Video;
	const auto video = file.information
		? std::get_if<Video>(&file.information->media)
		: nullptr;
	return video && (video->duration > kMaxVideoSeconds * crl::time(1000));
}

[[nodiscard]] QImage RenderPreview(const Ui::PreparedFile &file) {
	auto source = file.preview;
	if (source.isNull() && file.information) {
		using Image = Ui::PreparedFileInformation::Image;
		using Video = Ui::PreparedFileInformation::Video;
		const auto &media = file.information->media;
		if (const auto image = std::get_if<Image>(&media)) {
			source = image->data;
		} else if (const auto video = std::get_if<Video>(&media)) {
			source = video->thumbnail;
		}
	}
	const auto ratio = style::DevicePixelRatio();
	auto result = ComposeCanvas(source, st::sereinStoryPreviewSize * ratio);
	result.setDevicePixelRatio(ratio);
	return result;
}

void ChooseFile(
		not_null<Window::SessionController*> controller,
		Fn<void(Ui::PreparedFile)> chosen) {
	const auto premium = controller->session().premium();
	const auto callback = crl::guard(controller, [=](
			FileDialog::OpenResult &&result) {
		const auto check = [=](const Ui::PreparedList &list) {
			using Type = Ui::PreparedFile::Type;
			const auto ok = (list.files.size() == 1)
				&& ((list.files.front().type == Type::Photo)
					|| ((list.files.front().type == Type::Video)
						&& !list.files.front().isGifv()));
			if (!ok) {
				controller->showToast(
					tr::lng_serein_story_unsupported(tr::now));
			}
			return ok;
		};
		const auto error = [=](tr::phrase<> text) {
			controller->showToast(text(tr::now));
		};
		auto list = Storage::PreparedFileFromFilesDialog(
			std::move(result),
			check,
			error,
			st::sendMediaPreviewSize,
			premium);
		if (!list) {
			return;
		}
		auto &file = list->files.front();
		if (TooLong(file)) {
			controller->showToast(tr::lng_serein_story_video_too_long(
				tr::now,
				lt_seconds,
				QString::number(kMaxVideoSeconds)));
			return;
		}
		chosen(std::move(file));
	});
	FileDialog::GetOpenPath(
		Core::App().getFileDialogParent(),
		tr::lng_attach_photo_or_video(tr::now),
		FileDialog::PhotoVideoFilesFilter(),
		callback);
}

void AddPeriodSection(
		not_null<Ui::GenericBox*> box,
		not_null<ComposerState*> state) {
	Ui::AddSubsectionTitle(
		box->verticalLayout(),
		tr::lng_serein_story_period());
	const auto group = std::make_shared<Ui::RadiobuttonGroup>(state->period);
	for (const auto seconds : PeriodChoices(true)) {
		box->addRow(
			object_ptr<Ui::Radiobutton>(
				box,
				group,
				seconds,
				tr::lng_hours(tr::now, lt_count, seconds / kHour),
				st::settingsSendType),
			st::settingsSendTypePadding);
	}
	group->setChangedCallback([=](int seconds) {
		state->period = seconds;
	});
}

void ComposerBox(
		not_null<Ui::GenericBox*> box,
		not_null<Window::SessionController*> controller,
		not_null<PeerData*> peer,
		Ui::PreparedFile file) {
	const auto session = &controller->session();
	const auto state = box->lifetime().make_state<ComposerState>(session);
	box->setTitle(tr::lng_serein_story_new());
	box->setWidth(st::boxWideWidth);
	if (!peer->isSelf()) {
		box->addRow(object_ptr<Ui::FlatLabel>(
			box,
			tr::lng_serein_story_posting_as(tr::now, lt_name, peer->name()),
			st::boxDividerLabel));
	}

	const auto preview = box->addRow(
		object_ptr<Ui::RpWidget>(box),
		style::al_top);
	preview->resize(st::sereinStoryPreviewSize);
	preview->paintRequest() | rpl::on_next([=] {
		auto p = QPainter(preview);
		auto hq = PainterHighQualityEnabler(p);
		auto path = QPainterPath();
		path.addRoundedRect(
			preview->rect(),
			st::roundRadiusLarge,
			st::roundRadiusLarge);
		p.setClipPath(path);
		p.drawImage(preview->rect(), state->preview);
	}, preview->lifetime());
	const auto setFile = [=](Ui::PreparedFile chosen) {
		state->preview = RenderPreview(chosen);
		state->file.emplace(std::move(chosen));
		preview->update();
	};
	setFile(std::move(file));
	const auto change = box->addRow(
		object_ptr<Ui::LinkButton>(box, tr::lng_serein_story_change(tr::now)),
		style::al_top);
	change->setClickedCallback([=] {
		if (!state->posting) {
			ChooseFile(controller, crl::guard(box, setFile));
		}
	});

	const auto caption = box->addRow(object_ptr<Ui::InputField>(
		box,
		st::sereinStoryCaption,
		Ui::InputField::Mode::MultiLine,
		tr::lng_photo_caption()));
	InitMessageFieldHandlers(
		controller,
		caption,
		ChatHelpers::PauseReason::Layer);
	caption->setMaxLength(CaptionLimit(session));
	box->setFocusCallback([=] { caption->setFocusFast(); });

	if (peer->isSelf()) {
		AddAudienceSection(box->verticalLayout(), controller, &state->audience);
	}
	if (session->premium()) {
		AddPeriodSection(box, state);
	}
	Ui::AddSkip(box->verticalLayout());
	const auto pinned = box->addRow(object_ptr<Ui::Checkbox>(
		box,
		tr::lng_serein_story_pinned(tr::now),
		true));
	const auto screenshots = box->addRow(object_ptr<Ui::Checkbox>(
		box,
		tr::lng_serein_story_screenshots(tr::now),
		true));
	const auto status = box->addRow(
		object_ptr<Ui::SlideWrap<Ui::FlatLabel>>(
			box,
			object_ptr<Ui::FlatLabel>(box, QString(), st::boxDividerLabel)));
	status->hide(anim::type::instant);

	const auto post = [=] {
		if (state->posting || !state->file) {
			return;
		}
		auto rules = peer->isSelf()
			? AudienceRulesFor(state->audience.current())
			: AudienceRules(Audience::Everyone, {});
		if (rules.empty()) {
			box->showToast(tr::lng_serein_story_need_people(tr::now));
			return;
		}
		state->posting = true;
		status->show(anim::type::normal);
		state->publisher.start({
			.peer = peer,
			.caption = caption->getTextWithAppliedMarkdown(),
			.rules = std::move(rules),
			.period = EffectivePeriod(state->period, session->premium()),
			.pinned = pinned->checked(),
			.protect = !screenshots->checked(),
		}, *state->file, {
			.progress = [=](Stage stage, float64 value) {
				status->entity()->setText(StageText(stage, value));
			},
			.done = [=] {
				controller->showToast(tr::lng_serein_story_posted(tr::now));
				box->closeBox();
			},
			.fail = [=](const QString &type) {
				state->posting = false;
				status->hide(anim::type::normal);
				box->showToast(ErrorText(type));
			},
		});
	};
	box->addButton(tr::lng_serein_story_post(), post);
	box->addButton(tr::lng_cancel(), [=] { box->closeBox(); });
}

} // namespace

void StartPosting(
		not_null<Window::SessionController*> controller,
		not_null<PeerData*> peer) {
	controller->session().api().request(MTPstories_CanSendStory(
		peer->input()
	)).done(crl::guard(controller, [=] {
		ChooseFile(controller, [=](Ui::PreparedFile file) {
			controller->show(
				Box(ComposerBox, controller, peer, std::move(file)));
		});
	})).fail(crl::guard(controller, [=](const MTP::Error &error) {
		if (!MTP::IgnoreError(error)) {
			controller->showToast(ErrorText(error.type()));
		}
	})).handleFloodErrors().send();
}

} // namespace Serein::Stories
