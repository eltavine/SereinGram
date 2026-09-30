#include "serein/hooks/filters/view.h"

#include "serein/filters/model.h"

#include "data/data_peer.h"
#include "data/data_peer_id.h"
#include "data/data_groups.h"
#include "data/data_session.h"
#include "data/data_user.h"
#include "history/history.h"
#include "history/history_item.h"
#include "history/history_item_components.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QCache>

#include <array>

namespace Serein::Filters {
namespace {

struct Cached {
	FullMsgId id;
	QByteArray config;
	TextWithEntities source;
	QString searchable;
	QString author;
	QString peer;
	bool blocked = false;
	bool outgoing = false;
	Result result;
};

QCache<quintptr, Cached> &Results() {
	static auto result = QCache<quintptr, Cached>(4096);
	return result;
}

QString Searchable(not_null<HistoryItem*> item) {
	auto result = QString();
	const auto append = [&](const QString &text) {
		if (result.size() <= 16384) {
			result += text.left(16385 - result.size());
		}
	};
	const auto add = [&](not_null<HistoryItem*> part) {
		const auto &original = part->originalText();
		append(original.text);
		for (const auto &entity : original.entities) {
			if (entity.type() == EntityType::CustomUrl) {
				append(u"\n"_q + entity.data());
			}
		}
		if (const auto markup = part->Get<HistoryMessageReplyMarkup>()) {
			for (const auto &row : markup->data.rows) {
				for (const auto &button : row) {
					append(u"\n"_q + button.text + u" "_q
						+ QString::fromUtf8(button.data));
				}
			}
		}
	};
	if (const auto group = item->history()->owner().groups().find(item)) {
		for (const auto &part : group->items) {
			append(u"\n"_q);
			add(part);
		}
	} else {
		add(item);
	}
	return result;
}

[[nodiscard]] Result Project(
		HistoryItem *item,
		const TextWithEntities &source) {
	if (!item || item->isService() || item->sereinOriginalShown()) {
		return { .text = source };
	}
	const auto raw = ForAccount(&item->history()->session()).Get(kRules);
	if (raw.isEmpty()) {
		return { .text = source };
	}
	const auto config = QJsonDocument::fromJson(raw).object();
	const auto forwarded = item->Get<HistoryMessageForwarded>();
	const auto sources = std::array<PeerData*, 3>{
		item->from().get(),
		forwarded ? forwarded->originalSender : nullptr,
		item->viaBot(),
	};
	const auto hiddenAuthors = config.value(u"hiddenAuthors"_q).toArray();
	auto author = QString();
	auto blocked = false;
	for (const auto source : sources) {
		if (!source) {
			continue;
		}
		blocked = blocked || source->isBlocked();
		const auto id = QString::number(SerializePeerId(source->id));
		if (hiddenAuthors.contains(id)) {
			author = id;
		}
	}
	const auto peer = item->history()->peer;
	const auto peerId = QString::number(SerializePeerId(peer->id));
	const auto searchable = Searchable(item);
	const auto key = reinterpret_cast<quintptr>(item);
	if (const auto cached = Results().object(key);
		cached && cached->id == item->fullId()
		&& cached->config == raw
		&& cached->source.text == source.text
		&& cached->source.entities == source.entities
		&& cached->searchable == searchable
		&& cached->author == author && cached->peer == peerId
		&& cached->blocked == blocked && cached->outgoing == item->out()) {
		return cached->result;
	}
	const auto result = Apply(raw, source, author,
		peerId, blocked, item->out(), searchable);
	Results().insert(key, new Cached{
		.id = item->fullId(),
		.config = raw,
		.source = source,
		.searchable = searchable,
		.author = author,
		.peer = peerId,
		.blocked = blocked,
		.outgoing = item->out(),
		.result = result,
	});
	if (!result.error.isEmpty()) {
		LOG(("Serein filter: %1 (message %2).")
			.arg(result.error, QString::number(item->id.bare)));
	}
	return result;
}

[[nodiscard]] TextWithEntities DisplayText(const Result &result) {
	return result.hidden
		? TextWithEntities{ tr::lng_serein_filter_hidden(tr::now) }
		: result.text;
}

} // namespace
} // namespace Serein::Filters

namespace Serein::Hooks::Filters {

bool Hidden(HistoryItem *item) {
	return item && Serein::Filters::Project(item,
		item->translatedTextWithLocalEntities()).hidden;
}

TextWithEntities DisplayText(
		HistoryItem *item,
		const TextWithEntities &source) {
	using namespace Serein::Filters;
	return Serein::Filters::DisplayText(Project(item, source));
}

TextWithEntities ReplyText(
		HistoryItem *quoted,
		const TextWithEntities &text) {
	return Hidden(quoted)
		? TextWithEntities{ tr::lng_serein_filter_hidden(tr::now) }
		: text;
}

bool HiddenPeer(PeerData *peer) {
	if (!peer || !peer->isBlocked()) {
		return false;
	}
	const auto raw = ForAccount(&peer->session()).Get(
		Serein::Filters::kRules);
	return !raw.isEmpty() && QJsonDocument::fromJson(raw).object().value(
		u"hideBlocked"_q).toBool();
}

} // namespace Serein::Hooks::Filters
