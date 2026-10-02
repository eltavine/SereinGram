#include "serein/app/user_lookup.h"

#include "apiwrap.h"
#include "data/data_session.h"
#include "data/data_user.h"
#include "main/main_session.h"
#include "serein/core/options.h"
#include "serein/schema/gen/settings/media.h"

#include <QtCore/QRegularExpression>

namespace Serein {
namespace {

constexpr auto kMaxCandidates = 3;

[[nodiscard]] QString LookupBot() {
	const auto value = ForDevice().Get(Media::kStickerAuthorBot).trimmed();
	return value.startsWith(u'@') ? value.mid(1) : value;
}

[[nodiscard]] QStringList Candidates(const MTPmessages_BotResults &results) {
	static const auto pattern = QRegularExpression(
		u"(?:^|[^A-Za-z0-9_])(?:@|t\\.me/)([A-Za-z][A-Za-z0-9_]{3,31})"_q);
	auto result = QStringList();
	const auto scan = [&](const QString &text) {
		auto matches = pattern.globalMatch(text);
		while (matches.hasNext() && result.size() < kMaxCandidates) {
			const auto name = matches.next().captured(1);
			if (!result.contains(name, Qt::CaseInsensitive)) {
				result.push_back(name);
			}
		}
	};
	for (const auto &entry : results.data().vresults().v) {
		entry.match([&](const auto &data) {
			scan(qs(data.vtitle().value_or_empty()));
			scan(qs(data.vdescription().value_or_empty()));
			data.vsend_message().match([&](
					const MTPDbotInlineMessageText &message) {
				scan(qs(message.vmessage()));
			}, [&](const MTPDbotInlineMessageMediaAuto &message) {
				scan(qs(message.vmessage()));
			}, [](const auto &) {
			});
		});
	}
	return result;
}

void ResolveCandidates(
		not_null<Main::Session*> session,
		QStringList names,
		UserId id,
		Fn<void(UserData*)> done) {
	if (const auto user = session->data().userLoaded(id)) {
		done(user);
		return;
	} else if (names.isEmpty()) {
		done(nullptr);
		return;
	}
	const auto name = names.takeFirst();
	session->api().request(MTPcontacts_ResolveUsername(
		MTP_flags(0),
		MTP_string(name),
		MTP_string(QString())
	)).done([=](const MTPcontacts_ResolvedPeer &result) {
		const auto &data = result.data();
		session->data().processUsers(data.vusers());
		session->data().processChats(data.vchats());
		ResolveCandidates(session, names, id, done);
	}).fail([=] {
		ResolveCandidates(session, names, id, done);
	}).send();
}

void QueryBot(
		not_null<Main::Session*> session,
		not_null<UserData*> bot,
		UserId id,
		Fn<void(UserData*)> done) {
	session->api().request(MTPmessages_GetInlineBotResults(
		MTP_flags(0),
		bot->inputUser(),
		MTP_inputPeerSelf(),
		MTPInputGeoPoint(),
		MTP_string(QString::number(id.bare)),
		MTP_string(QString())
	)).done([=](const MTPmessages_BotResults &result) {
		session->data().processUsers(result.data().vusers());
		ResolveCandidates(session, Candidates(result), id, done);
	}).fail([=] {
		done(nullptr);
	}).send();
}

} // namespace

bool HasUserLookup() {
	return !LookupBot().isEmpty();
}

void LookupUser(
		not_null<Main::Session*> session,
		UserId id,
		Fn<void(UserData*)> done) {
	const auto name = LookupBot();
	if (name.isEmpty()) {
		done(nullptr);
		return;
	} else if (const auto peer = session->data().peerByUsername(name)) {
		const auto bot = peer->asUser();
		if (bot && bot->isBot()) {
			QueryBot(session, bot, id, std::move(done));
		} else {
			done(nullptr);
		}
		return;
	}
	session->api().request(MTPcontacts_ResolveUsername(
		MTP_flags(0),
		MTP_string(name),
		MTP_string(QString())
	)).done([=](const MTPcontacts_ResolvedPeer &result) {
		const auto &data = result.data();
		session->data().processUsers(data.vusers());
		session->data().processChats(data.vchats());
		const auto peer = session->data().peerLoaded(
			peerFromMTP(data.vpeer()));
		const auto bot = peer ? peer->asUser() : nullptr;
		if (bot && bot->isBot()) {
			QueryBot(session, bot, id, done);
		} else {
			done(nullptr);
		}
	}).fail([=] {
		done(nullptr);
	}).send();
}

} // namespace Serein
