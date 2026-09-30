#include "serein/hooks/history.h"

#include "serein/adapters/openssl/aes_gcm_cipher.h"
#include "serein/adapters/qtsql/history_store.h"
#include "serein/features/history/model/recorder.h"
#include "base/unixtime.h"
#include "data/data_media_types.h"
#include "data/data_peer.h"
#include "data/data_user.h"
#include "history/history.h"
#include "history/history_item.h"
#include "logs.h"
#include "main/main_session.h"
#include "mtproto/mtproto_auth_key.h"
#include "storage/storage_account.h"
#include "ui/text/text_entity.h"

#include <QtCore/QFileInfo>

#include <map>

namespace Serein::Hooks {
namespace {

struct Backend {
	std::unique_ptr<Adapters::AesGcmCipher> cipher;
	std::unique_ptr<Adapters::SqlHistoryStore> store;
	std::unique_ptr<HistoryFeature::Recorder> recorder;
};

[[nodiscard]] QString EntityName(EntityType type) {
	switch (type) {
	case EntityType::Url: return u"url"_q;
	case EntityType::CustomUrl: return u"custom_url"_q;
	case EntityType::Email: return u"email"_q;
	case EntityType::Hashtag: return u"hashtag"_q;
	case EntityType::Cashtag: return u"cashtag"_q;
	case EntityType::Mention: return u"mention"_q;
	case EntityType::MentionName: return u"mention_name"_q;
	case EntityType::CustomEmoji: return u"custom_emoji"_q;
	case EntityType::BotCommand: return u"bot_command"_q;
	case EntityType::MediaTimestamp: return u"media_timestamp"_q;
	case EntityType::Phone: return u"phone"_q;
	case EntityType::BankCard: return u"bank_card"_q;
	case EntityType::Bold: return u"bold"_q;
	case EntityType::Semibold: return u"semibold"_q;
	case EntityType::Italic: return u"italic"_q;
	case EntityType::Underline: return u"underline"_q;
	case EntityType::StrikeOut: return u"strike"_q;
	case EntityType::Code: return u"code"_q;
	case EntityType::Pre: return u"pre"_q;
	case EntityType::Blockquote: return u"blockquote"_q;
	case EntityType::Spoiler: return u"spoiler"_q;
	case EntityType::Subscript: return u"subscript"_q;
	case EntityType::Superscript: return u"superscript"_q;
	case EntityType::FormattedDate: return u"formatted_date"_q;
	default: return QString();
	}
}

[[nodiscard]] HistoryFeature::Snapshot TakeSnapshot(not_null<HistoryItem*> item) {
	auto result = HistoryFeature::Snapshot();
	result.peerId = qint64(item->history()->peer->id.value);
	result.messageId = item->id.bare;
	result.topicRootId = item->topicRootId().bare;
	result.date = item->date();
	const auto from = item->from();
	result.fromPeerId = qint64(from->id.value);
	const auto user = from->asUser();
	result.fromBot = user && user->isBot();
	const auto &text = item->originalText();
	result.text = text.text;
	for (const auto &entity : text.entities) {
		const auto name = EntityName(entity.type());
		if (!name.isEmpty() && entity.offset() >= 0 && entity.length() > 0) {
			result.entities.push_back({
				name,
				entity.offset(),
				entity.length(),
				entity.data(),
			});
		}
	}
	if (const auto media = item->media()) {
		result.mediaSummary = media->notificationText().text;
	}
	return result;
}

[[nodiscard]] std::unique_ptr<Backend> OpenBackend(
		not_null<Main::Session*> session) {
	const auto key = session->local().peekLegacyLocalKey();
	if (!key) {
		return nullptr;
	}
	const auto bytes = key->data();
	auto backend = std::make_unique<Backend>();
	backend->cipher = Adapters::AesGcmCipher::FromSecret(
		QByteArray(reinterpret_cast<const char*>(bytes.data()), int(bytes.size())),
		"serein-history-v1");
	if (!backend->cipher) {
		return nullptr;
	}
	const auto directory = QFileInfo(session->local().supportModePath()).absolutePath();
	auto error = QString();
	backend->store = Adapters::SqlHistoryStore::Open(
		directory + u"/serein_history.sqlite3"_q,
		*backend->cipher,
		&error);
	if (!backend->store) {
		LOG(("Serein History Error: %1").arg(error));
		return nullptr;
	}
	backend->recorder = std::make_unique<HistoryFeature::Recorder>(
		*backend->store,
		[] { return qint64(base::unixtime::now()); });
	backend->recorder->prune(HistoryFeature::Read(ForAccount(session)));
	return backend;
}

[[nodiscard]] HistoryFeature::Recorder *RecorderFor(not_null<Main::Session*> session) {
	static auto backends = std::map<Main::Session*, std::unique_ptr<Backend>>();
	const auto i = backends.find(session);
	if (i != backends.end()) {
		return i->second ? i->second->recorder.get() : nullptr;
	}
	auto &slot = backends[session];
	slot = OpenBackend(session);
	session->lifetime().add([=] { backends.erase(session); });
	return slot ? slot->recorder.get() : nullptr;
}

} // namespace

void OnServerDeleted(const std::vector<gsl::not_null<HistoryItem*>> &items) {
	for (const auto &item : items) {
		const auto session = &item->history()->session();
		const auto policy = HistoryFeature::Read(ForAccount(session));
		if (!policy.saveDeleted) {
			continue;
		} else if (const auto recorder = RecorderFor(session)) {
			recorder->recordDeleted(policy, TakeSnapshot(item));
		}
	}
}

void OnBeforeEdition(
		gsl::not_null<HistoryItem*> item,
		const TextWithEntities &updated) {
	const auto session = &item->history()->session();
	const auto policy = HistoryFeature::Read(ForAccount(session));
	if (!policy.saveEdits || item->originalText() == updated) {
		return;
	} else if (const auto recorder = RecorderFor(session)) {
		recorder->recordEdit(policy, TakeSnapshot(item));
	}
}

} // namespace Serein::Hooks
