#pragma once

#include "serein/features/stories/model/post.h"
#include "base/weak_ptr.h"
#include "data/data_msg_id.h"

class PeerData;
class TaskQueue;
struct FilePrepareResult;

namespace Api {
struct RemoteFileInfo;
} // namespace Api

namespace Main {
class Session;
} // namespace Main

namespace Ui {
struct PreparedFile;
} // namespace Ui

namespace Serein::Stories {

struct Repost {
	not_null<PeerData*> from;
	StoryId story = 0;
};

struct Post {
	not_null<PeerData*> peer;
	TextWithTags caption;
	std::vector<AudienceRule> rules;
	int period = kDefaultPeriod;
	bool pinned = true;
	bool protect = false;
	std::optional<Repost> repost;
};

enum class Stage {
	Preparing,
	Uploading,
	Publishing,
};

class Publisher final : public base::has_weak_ptr {
public:
	struct Callbacks {
		Fn<void(Stage, float64)> progress;
		Fn<void()> done;
		Fn<void(QString)> fail;
	};

	explicit Publisher(not_null<Main::Session*> session);
	~Publisher();

	void start(
		Post &&post,
		const Ui::PreparedFile &file,
		Callbacks &&callbacks);
	void repost(
		Post &&post,
		const MTPInputMedia &media,
		Callbacks &&callbacks);

private:
	void composed(QImage canvas, QByteArray jpeg);
	void upload(const std::shared_ptr<FilePrepareResult> &prepared);
	void subscribeToUploader();
	[[nodiscard]] float64 uploadedPart() const;
	void uploaded(const Api::RemoteFileInfo &info);
	void send(const MTPInputMedia &media);
	void progress(Stage stage, float64 value);
	void fail(const QString &type);
	void cancelUpload();

	const not_null<Main::Session*> _session;
	std::unique_ptr<TaskQueue> _queue;
	std::optional<Post> _post;
	Callbacks _callbacks;
	mtpRequestId _requestId = 0;
	bool _photo = false;
	FullMsgId _uploadId;
	uint64 _mediaId = 0;
	QString _mime;
	QVector<MTPDocumentAttribute> _attributes;
	rpl::lifetime _uploadLifetime;

};

} // namespace Serein::Stories
