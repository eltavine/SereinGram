#include "serein/features/stories/publisher.h"

#include "serein/features/stories/canvas.h"
#include "api/api_common.h"
#include "api/api_text_entities.h"
#include "apiwrap.h"
#include "base/random.h"
#include "data/data_document.h"
#include "data/data_peer.h"
#include "data/data_photo.h"
#include "data/data_session.h"
#include "data/data_user.h"
#include "main/main_session.h"
#include "storage/file_upload.h"
#include "storage/localimageloader.h"
#include "ui/chat/attach/attach_prepare.h"
#include "ui/image/image_prepare.h"
#include "ui/text/text_entity.h"

namespace Serein::Stories {
namespace {

using Prepared = std::shared_ptr<FilePrepareResult>;

class PrepareTask final : public Task {
public:
	PrepareTask(FileLoadTask::Args &&args, Fn<void(Prepared)> done)
	: _task(std::move(args))
	, _done(std::move(done)) {
	}

	void process() override {
		_task.process({ .generateGoodThumbnail = false });
	}
	void finish() override {
		_done(_task.peekResult());
	}

private:
	FileLoadTask _task;
	Fn<void(Prepared)> _done;

};

[[nodiscard]] std::unique_ptr<Task> MakeTask(
		FileLoadTask::Args &&args,
		Fn<void(Prepared)> done) {
	return std::make_unique<PrepareTask>(std::move(args), std::move(done));
}

[[nodiscard]] FileLoadTo LoadTo(not_null<PeerData*> peer) {
	return FileLoadTo(peer->id, Api::SendOptions(), FullReplyTo(), MsgId());
}

[[nodiscard]] TextWithEntities PrepareCaption(const TextWithTags &caption) {
	auto result = TextWithEntities{
		caption.text,
		TextUtilities::ConvertTextTagsToEntities(caption.tags),
	};
	TextUtilities::PrepareForSending(
		result,
		TextParseLinks | TextParseMentions | TextParseHashtags);
	TextUtilities::Trim(result);
	return result;
}

[[nodiscard]] MTPInputPrivacyRule PrivacyRule(
		not_null<Main::Session*> session,
		const AudienceRule &rule) {
	const auto users = [&] {
		auto result = QVector<MTPInputUser>();
		result.reserve(int(rule.users.size()));
		for (const auto id : rule.users) {
			result.push_back(session->data().user(UserId(id))->inputUser());
		}
		return MTP_vector<MTPInputUser>(std::move(result));
	};
	using Kind = AudienceRule::Kind;
	switch (rule.kind) {
	case Kind::AllowAll: return MTP_inputPrivacyValueAllowAll();
	case Kind::AllowContacts: return MTP_inputPrivacyValueAllowContacts();
	case Kind::AllowCloseFriends:
		return MTP_inputPrivacyValueAllowCloseFriends();
	case Kind::AllowUsers: return MTP_inputPrivacyValueAllowUsers(users());
	case Kind::DisallowUsers:
		return MTP_inputPrivacyValueDisallowUsers(users());
	}
	Unexpected("Rule kind in Serein::Stories::PrivacyRule.");
}

}

Publisher::Publisher(not_null<Main::Session*> session)
: _session(session)
, _queue(std::make_unique<TaskQueue>()) {
}

Publisher::~Publisher() {
	cancelUpload();
	if (_requestId) {
		_session->api().request(base::take(_requestId)).cancel();
	}
}

void Publisher::start(
		Post &&post,
		const Ui::PreparedFile &file,
		Callbacks &&callbacks) {
	if (_post) {
		return;
	}
	_post = std::move(post);
	_callbacks = std::move(callbacks);
	_photo = (file.type == Ui::PreparedFile::Type::Photo);
	progress(Stage::Preparing, 0.);
	if (!_photo) {
		auto information = file.information
			? std::make_unique<Ui::PreparedFileInformation>(*file.information)
			: nullptr;
		_queue->addTask(MakeTask({
			.session = _session,
			.filepath = file.path,
			.content = file.content,
			.information = std::move(information),
			.type = SendMediaType::File,
			.to = LoadTo(_post->peer),
			.displayName = file.displayName,
		}, crl::guard(this, [=](Prepared prepared) {
			upload(prepared);
		})));
		return;
	}
	auto image = QImage();
	if (file.information) {
		using Image = Ui::PreparedFileInformation::Image;
		if (const auto data = std::get_if<Image>(&file.information->media)) {
			image = data->data;
		}
	}
	crl::async([
			image,
			weak = base::make_weak(this),
			path = file.path,
			content = file.content] {
		const auto source = !image.isNull()
			? image
			: Images::Read({ .path = path, .content = content }).image;
		auto canvas = ComposeCanvas(source, kCanvasSize);
		auto jpeg = EncodeJpeg(canvas);
		crl::on_main(weak, [
				weak,
				canvas = std::move(canvas),
				jpeg = std::move(jpeg)]() mutable {
			weak->composed(std::move(canvas), std::move(jpeg));
		});
	});
}

void Publisher::composed(QImage canvas, QByteArray jpeg) {
	if (!_post) {
		return;
	} else if (jpeg.isEmpty()) {
		fail(QString());
		return;
	}
	auto information = std::make_unique<Ui::PreparedFileInformation>();
	information->filemime = u"image/jpeg"_q;
	information->media = Ui::PreparedFileInformation::Image{
		.data = std::move(canvas),
		.bytes = jpeg,
		.format = "jpeg",
	};
	_queue->addTask(MakeTask({
		.session = _session,
		.content = std::move(jpeg),
		.information = std::move(information),
		.type = SendMediaType::Photo,
		.to = LoadTo(_post->peer),
		.sendLargePhotos = true,
	}, crl::guard(this, [=](Prepared prepared) {
		upload(prepared);
	})));
}

void Publisher::upload(const Prepared &prepared) {
	if (!_post) {
		return;
	}
	const auto type = _photo ? SendMediaType::Photo : SendMediaType::File;
	if (!prepared || prepared->type != type || prepared->filesize <= 0) {
		fail(QString());
		return;
	}
	if (!_photo) {
		_mime = prepared->filemime;
		prepared->document.match([&](const MTPDdocument &data) {
			_attributes = data.vattributes().v;
		}, [](const auto &) {
		});
	}
	_mediaId = prepared->id;
	_uploadId = FullMsgId(
		_post->peer->id,
		_session->data().nextLocalMessageId());
	subscribeToUploader();
	progress(Stage::Uploading, 0.);
	_session->uploader().upload(_uploadId, prepared);
}

void Publisher::subscribeToUploader() {
	_uploadLifetime.destroy();
	const auto &uploader = _session->uploader();
	const auto mine = [=](const FullMsgId &id) {
		return _uploadId && (id == _uploadId);
	};
	(_photo ? uploader.photoReady() : uploader.documentReady())
	| rpl::filter([=](const Storage::UploadedMedia &data) {
		return mine(data.fullId);
	}) | rpl::on_next([=](const Storage::UploadedMedia &data) {
		uploaded(data.info);
	}, _uploadLifetime);

	(_photo ? uploader.photoProgress() : uploader.documentProgress())
	| rpl::filter(mine) | rpl::on_next([=] {
		progress(Stage::Uploading, uploadedPart());
	}, _uploadLifetime);

	(_photo ? uploader.photoFailed() : uploader.documentFailed())
	| rpl::filter(mine) | rpl::on_next([=] {
		_uploadId = FullMsgId();
		fail(QString());
	}, _uploadLifetime);
}

float64 Publisher::uploadedPart() const {
	return _photo
		? _session->data().photo(_mediaId)->progress()
		: _session->data().document(_mediaId)->progress();
}

void Publisher::uploaded(const Api::RemoteFileInfo &info) {
	_uploadId = FullMsgId();
	if (_photo) {
		send(MTP_inputMediaUploadedPhoto(
			MTP_flags(0),
			info.file,
			MTPVector<MTPInputDocument>(),
			MTPint(),
			MTPInputDocument()));
		return;
	}
	using Flag = MTPDinputMediaUploadedDocument::Flag;
	send(MTP_inputMediaUploadedDocument(
		MTP_flags(info.thumb ? Flag::f_thumb : Flag()),
		info.file,
		info.thumb.value_or(MTPInputFile()),
		MTP_string(_mime),
		MTP_vector<MTPDocumentAttribute>(_attributes),
		MTPVector<MTPInputDocument>(),
		MTPInputPhoto(),
		MTPint(),
		MTPint()));
}

void Publisher::send(const MTPInputMedia &media) {
	if (!_post) {
		return;
	}
	progress(Stage::Publishing, 1.);
	const auto &post = *_post;
	const auto caption = PrepareCaption(post.caption);
	const auto entities = Api::EntitiesToMTP(
		_session,
		caption.entities,
		Api::ConvertOption::SkipLocal);
	auto rules = QVector<MTPInputPrivacyRule>();
	rules.reserve(int(post.rules.size()));
	for (const auto &rule : post.rules) {
		rules.push_back(PrivacyRule(_session, rule));
	}
	using Flag = MTPstories_SendStory::Flag;
	const auto flags = (caption.text.isEmpty() ? Flag() : Flag::f_caption)
		| (entities.v.isEmpty() ? Flag() : Flag::f_entities)
		| (post.pinned ? Flag::f_pinned : Flag())
		| (post.protect ? Flag::f_noforwards : Flag())
		| ((post.period != kDefaultPeriod) ? Flag::f_period : Flag());
	_requestId = _session->api().request(MTPstories_SendStory(
		MTP_flags(flags),
		post.peer->input(),
		media,
		MTPVector<MTPMediaArea>(),
		MTP_string(caption.text),
		entities,
		MTP_vector<MTPInputPrivacyRule>(std::move(rules)),
		MTP_long(base::RandomValue<uint64>()),
		MTP_int(post.period),
		MTPInputPeer(),
		MTPint(),
		MTPVector<MTPint>(),
		MTPInputDocument()
	)).done([=](const MTPUpdates &result) {
		_requestId = 0;
		_session->api().applyUpdates(result);
		if (const auto done = _callbacks.done) {
			done();
		}
	}).fail([=](const MTP::Error &error) {
		_requestId = 0;
		fail(error.type());
	}).handleFloodErrors().send();
}

void Publisher::progress(Stage stage, float64 value) {
	if (const auto callback = _callbacks.progress) {
		callback(stage, value);
	}
}

void Publisher::fail(const QString &type) {
	cancelUpload();
	_post.reset();
	if (const auto callback = _callbacks.fail) {
		callback(type);
	}
}

void Publisher::cancelUpload() {
	if (const auto id = base::take(_uploadId)) {
		_session->uploader().cancel(id);
	}
}

} // namespace Serein::Stories
