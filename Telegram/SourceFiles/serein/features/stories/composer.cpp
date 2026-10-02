#include "serein/features/stories/composer.h"

#include "serein/features/stories/audience.h"
#include "serein/features/stories/canvas.h"
#include "serein/features/stories/common.h"
#include "serein/features/stories/publisher.h"
#include "apiwrap.h"
#include "chat_helpers/compose/compose_show.h"
#include "core/application.h"
#include "core/file_utilities.h"
#include "core/mime_type.h"
#include "data/data_document.h"
#include "data/data_document_media.h"
#include "data/data_file_origin.h"
#include "data/data_peer.h"
#include "data/data_photo.h"
#include "data/data_photo_media.h"
#include "data/data_story.h"
#include "data/data_user.h"
#include "editor/video/video_editor.h"
#include "editor/video/video_editor_layer.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "mainwindow.h"
#include "mtproto/mtproto_response.h"
#include "storage/storage_media_prepare.h"
#include "ui/chat/attach/attach_prepare.h"
#include "ui/image/image.h"
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

#include <QtCore/QMimeData>
#include <QtGui/QPainterPath>

#include <cmath>

namespace Serein::Stories {
namespace {

constexpr auto kHour = 3600;

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

struct PostControls {
	not_null<Ui::InputField*> caption;
	not_null<Ui::Checkbox*> pinned;
	not_null<Ui::Checkbox*> screenshots;
	not_null<Ui::SlideWrap<Ui::FlatLabel>*> status;
};

struct RepostSource {
	not_null<PeerData*> from;
	StoryId story = 0;
	MTPInputMedia media;
	std::shared_ptr<Data::PhotoMedia> photo;
	std::shared_ptr<Data::DocumentMedia> document;
};

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

[[nodiscard]] bool TooLong(const Ui::PreparedFile &file) {
	using Video = Ui::PreparedFileInformation::Video;
	const auto video = file.information
		? std::get_if<Video>(&file.information->media)
		: nullptr;
	return video && (video->duration > kMaxVideoSeconds * crl::time(1000));
}

[[nodiscard]] QImage RenderPreview(const QImage &source) {
	const auto ratio = style::DevicePixelRatio();
	auto result = ComposeCanvas(source, st::sereinStoryPreviewSize * ratio);
	result.setDevicePixelRatio(ratio);
	return result;
}

[[nodiscard]] QImage PreviewSource(const Ui::PreparedFile &file) {
	if (!file.preview.isNull() || !file.information) {
		return file.preview;
	}
	using Image = Ui::PreparedFileInformation::Image;
	using Video = Ui::PreparedFileInformation::Video;
	const auto &media = file.information->media;
	if (const auto image = std::get_if<Image>(&media)) {
		return image->data;
	} else if (const auto video = std::get_if<Video>(&media)) {
		return video->thumbnail;
	}
	return QImage();
}

[[nodiscard]] QImage PreviewSource(const RepostSource &source) {
	const auto image = source.photo
		? source.photo->image(Data::PhotoSize::Large)
		: source.document
		? source.document->thumbnail()
		: nullptr;
	return image ? image->original() : QImage();
}

void TrimVideo(
		std::shared_ptr<ChatHelpers::Show> show,
		Ui::PreparedFile file,
		Fn<void(Ui::PreparedFile)> chosen) {
	using Video = Ui::PreparedFileInformation::Video;
	const auto window = show->resolveWindow();
	const auto video = file.information
		? std::get_if<Video>(&file.information->media)
		: nullptr;
	const auto seconds = QString::number(kMaxVideoSeconds);
	if (!window || !video || video->thumbnail.isNull()) {
		show->showToast(tr::lng_serein_story_video_too_long(
			tr::now,
			lt_seconds,
			seconds));
		return;
	}
	auto descriptor = Editor::VideoEditorDescriptor{
		.path = file.path,
		.content = file.content,
		.dimensions = video->thumbnail.size(),
		.duration = video->duration,
		.data = Editor::VideoEditorData{
			.hint = tr::lng_serein_story_trim_hint(
				tr::now,
				lt_seconds,
				seconds),
			.maxDuration = kMaxVideoSeconds * crl::time(1000),
		},
		.initial = video->modifications,
	};
	const auto shared = std::make_shared<Ui::PreparedFile>(std::move(file));
	Editor::ShowVideoEditorLayer(
		window->widget(),
		&window->window(),
		std::move(descriptor),
		[=](Editor::VideoModifications modifications) {
			auto &media = shared->information->media;
			std::get<Video>(media).modifications = modifications;
			chosen(std::move(*shared));
		});
}

[[nodiscard]] bool Postable(const Ui::PreparedList &list) {
	using Type = Ui::PreparedFile::Type;
	if (list.error != Ui::PreparedList::Error::None
		|| list.files.size() != 1) {
		return false;
	}
	const auto &file = list.files.front();
	return (file.type == Type::Photo)
		|| ((file.type == Type::Video) && !file.isGifv());
}

[[nodiscard]] Ui::PreparedList ListFromMimeData(
		not_null<const QMimeData*> data,
		bool premium) {
	const auto urls = Core::ReadMimeUrls(data);
	if (!urls.isEmpty()) {
		return Storage::PrepareMediaList(
			urls.mid(0, 1),
			st::sendMediaPreviewSize,
			premium);
	} else if (auto read = Core::ReadMimeImage(data)) {
		return Storage::PrepareMediaFromImage(
			std::move(read.image),
			std::move(read.content),
			st::sendMediaPreviewSize);
	}
	return Ui::PreparedList(Ui::PreparedList::Error::EmptyFile, QString());
}

void Accept(
		std::shared_ptr<ChatHelpers::Show> show,
		Ui::PreparedFile file,
		Fn<void(Ui::PreparedFile)> chosen) {
	if (TooLong(file)) {
		TrimVideo(show, std::move(file), chosen);
	} else {
		chosen(std::move(file));
	}
}

void ChooseFile(
		std::shared_ptr<ChatHelpers::Show> show,
		Fn<void(Ui::PreparedFile)> chosen) {
	const auto premium = show->session().premium();
	const auto callback = [=](FileDialog::OpenResult &&result) {
		const auto check = [=](const Ui::PreparedList &list) {
			const auto ok = Postable(list);
			if (!ok) {
				show->showToast(tr::lng_serein_story_unsupported(tr::now));
			}
			return ok;
		};
		const auto error = [=](tr::phrase<> text) {
			show->showToast(text(tr::now));
		};
		auto list = Storage::PreparedFileFromFilesDialog(
			std::move(result),
			check,
			error,
			st::sendMediaPreviewSize,
			premium);
		if (list) {
			Accept(show, std::move(list->files.front()), chosen);
		}
	};
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

[[nodiscard]] not_null<Ui::RpWidget*> AddPreview(
		not_null<Ui::GenericBox*> box,
		not_null<ComposerState*> state) {
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
		p.fillRect(preview->rect(), Qt::black);
		p.drawImage(preview->rect(), state->preview);
	}, preview->lifetime());
	return preview;
}

[[nodiscard]] PostControls AddPostControls(
		not_null<Ui::GenericBox*> box,
		std::shared_ptr<ChatHelpers::Show> show,
		not_null<PeerData*> peer,
		not_null<ComposerState*> state) {
	const auto caption = AddCaptionField(box, show);
	if (peer->isSelf()) {
		AddAudienceSection(box->verticalLayout(), show, &state->audience);
	}
	if (show->session().premium()) {
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
	return { caption, pinned, screenshots, status };
}

[[nodiscard]] std::optional<Post> CollectPost(
		not_null<Ui::GenericBox*> box,
		not_null<PeerData*> peer,
		not_null<ComposerState*> state,
		const PostControls &controls) {
	auto rules = peer->isSelf()
		? AudienceRulesFor(state->audience.current())
		: AudienceRules(Audience::Everyone, {});
	if (rules.empty()) {
		box->showToast(tr::lng_serein_story_need_people(tr::now));
		return std::nullopt;
	}
	const auto premium = peer->session().premium();
	return Post{
		.peer = peer,
		.caption = controls.caption->getTextWithAppliedMarkdown(),
		.rules = std::move(rules),
		.period = EffectivePeriod(state->period, premium),
		.pinned = controls.pinned->checked(),
		.protect = !controls.screenshots->checked(),
	};
}

[[nodiscard]] Publisher::Callbacks PostCallbacks(
		not_null<Ui::GenericBox*> box,
		std::shared_ptr<ChatHelpers::Show> show,
		not_null<ComposerState*> state,
		const PostControls &controls) {
	const auto status = controls.status;
	return {
		.progress = [=](Stage stage, float64 value) {
			status->entity()->setText(StageText(stage, value));
		},
		.done = [=] {
			show->showToast(tr::lng_serein_story_posted(tr::now));
			box->closeBox();
		},
		.fail = [=](const QString &type) {
			state->posting = false;
			status->hide(anim::type::normal);
			box->showToast(ErrorText(type));
		},
	};
}

void ComposerBox(
		not_null<Ui::GenericBox*> box,
		std::shared_ptr<ChatHelpers::Show> show,
		not_null<PeerData*> peer,
		Ui::PreparedFile file) {
	const auto state = box->lifetime().make_state<ComposerState>(
		&show->session());
	box->setTitle(tr::lng_serein_story_new());
	box->setWidth(st::boxWideWidth);
	if (!peer->isSelf()) {
		box->addRow(object_ptr<Ui::FlatLabel>(
			box,
			tr::lng_serein_story_posting_as(tr::now, lt_name, peer->name()),
			st::boxDividerLabel));
	}
	const auto preview = AddPreview(box, state);
	const auto setFile = [=](Ui::PreparedFile chosen) {
		state->preview = RenderPreview(PreviewSource(chosen));
		state->file.emplace(std::move(chosen));
		preview->update();
	};
	setFile(std::move(file));
	const auto change = box->addRow(
		object_ptr<Ui::LinkButton>(box, tr::lng_serein_story_change(tr::now)),
		style::al_top);
	change->setClickedCallback([=] {
		if (!state->posting) {
			ChooseFile(show, crl::guard(box, setFile));
		}
	});
	const auto controls = AddPostControls(box, show, peer, state);
	controls.caption->setMimeDataHook([=](
			not_null<const QMimeData*> data,
			Ui::InputField::MimeAction action) {
		if (action == Ui::InputField::MimeAction::Check) {
			return data->hasImage() || data->hasUrls();
		} else if (state->posting) {
			return false;
		}
		auto list = ListFromMimeData(data, show->session().premium());
		if (list.error != Ui::PreparedList::Error::None) {
			return false;
		} else if (!Postable(list)) {
			box->showToast(tr::lng_serein_story_unsupported(tr::now));
			return true;
		}
		Accept(show, std::move(list.files.front()), crl::guard(box, setFile));
		return true;
	});
	box->addButton(tr::lng_serein_story_post(), [=] {
		if (state->posting || !state->file) {
			return;
		}
		auto post = CollectPost(box, peer, state, controls);
		if (!post) {
			return;
		}
		state->posting = true;
		controls.status->show(anim::type::normal);
		state->publisher.start(
			std::move(*post),
			*state->file,
			PostCallbacks(box, show, state, controls));
	});
	box->addButton(tr::lng_cancel(), [=] { box->closeBox(); });
}

void RepostBox(
		not_null<Ui::GenericBox*> box,
		std::shared_ptr<ChatHelpers::Show> show,
		RepostSource source) {
	const auto session = &show->session();
	const auto peer = not_null<PeerData*>(session->user());
	const auto state = box->lifetime().make_state<ComposerState>(session);
	box->setTitle(tr::lng_serein_story_repost());
	box->setWidth(st::boxWideWidth);
	const auto preview = AddPreview(box, state);
	const auto refresh = [=] {
		if (!state->preview.isNull()) {
			return;
		}
		if (const auto frame = PreviewSource(source); !frame.isNull()) {
			state->preview = RenderPreview(frame);
			preview->update();
		}
	};
	refresh();
	session->downloaderTaskFinished(
	) | rpl::on_next(refresh, preview->lifetime());
	const auto controls = AddPostControls(box, show, peer, state);
	box->addButton(tr::lng_serein_story_post(), [=] {
		if (state->posting) {
			return;
		}
		auto post = CollectPost(box, peer, state, controls);
		if (!post) {
			return;
		}
		post->repost = Repost{ source.from, source.story };
		state->posting = true;
		controls.status->show(anim::type::normal);
		state->publisher.repost(
			std::move(*post),
			source.media,
			PostCallbacks(box, show, state, controls));
	});
	box->addButton(tr::lng_cancel(), [=] { box->closeBox(); });
}

[[nodiscard]] std::optional<RepostSource> MakeRepostSource(
		not_null<Data::Story*> story) {
	const auto peer = story->peer();
	const auto origin = Data::FileOrigin(
		Data::FileOriginStory{ peer->id, story->id() });
	auto result = RepostSource{ .from = peer, .story = story->id() };
	if (const auto photo = story->photo()) {
		result.media = MTP_inputMediaPhoto(
			MTP_flags(0),
			photo->mtpInput(),
			MTPint(),
			MTPInputDocument());
		result.photo = photo->createMediaView();
		result.photo->wanted(Data::PhotoSize::Large, origin);
	} else if (const auto document = story->document()) {
		result.media = MTP_inputMediaDocument(
			MTP_flags(0),
			document->mtpInput(),
			MTPInputPhoto(),
			MTPint(),
			MTPint(),
			MTPstring());
		result.document = document->createMediaView();
		result.document->thumbnailWanted(origin);
	} else {
		return std::nullopt;
	}
	return result;
}

} // namespace

void StartPosting(
		std::shared_ptr<ChatHelpers::Show> show,
		not_null<PeerData*> peer) {
	show->session().api().request(MTPstories_CanSendStory(
		peer->input()
	)).done([=] {
		ChooseFile(show, [=](Ui::PreparedFile file) {
			show->showBox(Box(ComposerBox, show, peer, std::move(file)));
		});
	}).fail([=](const MTP::Error &error) {
		if (!MTP::IgnoreError(error)) {
			show->showToast(ErrorText(error.type()));
		}
	}).handleFloodErrors().send();
}

void StartRepost(
		std::shared_ptr<ChatHelpers::Show> show,
		not_null<Data::Story*> story) {
	const auto source = MakeRepostSource(story);
	if (!source) {
		return;
	}
	show->session().api().request(MTPstories_CanSendStory(
		MTP_inputPeerSelf()
	)).done([=] {
		show->showBox(Box(RepostBox, show, *source));
	}).fail([=](const MTP::Error &error) {
		if (!MTP::IgnoreError(error)) {
			show->showToast(ErrorText(error.type()));
		}
	}).handleFloodErrors().send();
}

} // namespace Serein::Stories
