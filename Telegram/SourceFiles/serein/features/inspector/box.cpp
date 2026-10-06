#include "serein/features/inspector/box.h"

#include "serein/display/icon_tile.h"
#include "serein/features/inspector/model/facts.h"
#include "serein/features/inspector/model/render.h"
#include "serein/features/inspector/sources.h"
#include "base/unixtime.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "history/history.h"
#include "history/history_item.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "mtproto/sender.h"
#include "ui/layers/generic_box.h"
#include "ui/text/format_values.h"
#include "ui/text/text_entity.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/labels.h"
#include "ui/wrap/slide_wrap.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"
#include "styles/style_layers.h"
#include "styles/style_menu_icons.h"
#include "styles/style_serein.h"
#include "styles/style_settings.h"

#include <QtCore/QLocale>

#include <array>

namespace Serein::Inspector {
namespace {

constexpr auto kSections = std::array{
	Section::Overview,
	Section::Chat,
	Section::Reply,
	Section::Forward,
	Section::Media,
	Section::Counters,
};

using Loader = Fn<void(MTP::Sender&, Fn<void(Snapshot)>)>;

struct Shown {
	QString text;
	QString copy;
};

[[nodiscard]] tr::phrase<> SectionTitle(Section section) {
	switch (section) {
	case Section::Overview: return tr::lng_serein_inspector_overview;
	case Section::Reply: return tr::lng_serein_inspector_reply;
	case Section::Forward: return tr::lng_serein_inspector_forward;
	case Section::Media: return tr::lng_serein_inspector_media;
	case Section::Counters: return tr::lng_serein_inspector_counters;
	case Section::Chat: return tr::lng_serein_inspector_chat;
	}
	Unexpected("Section in Inspector::SectionTitle.");
}

[[nodiscard]] const style::color &SectionTile(Section section) {
	switch (section) {
	case Section::Overview: return st::settingsIconBg4;
	case Section::Reply: return st::settingsIconBg2;
	case Section::Forward: return st::settingsIconBg5;
	case Section::Media: return st::settingsIconBg3;
	case Section::Counters: return st::settingsIconBg6;
	case Section::Chat: return st::settingsIconBg1;
	}
	Unexpected("Section in Inspector::SectionTile.");
}

[[nodiscard]] tr::phrase<> LabelText(Label label) {
	switch (label) {
	case Label::MessageId: return tr::lng_serein_details_message_id;
	case Label::Chat: return tr::lng_serein_details_chat_id;
	case Label::Sender: return tr::lng_serein_details_sender_id;
	case Label::Date: return tr::lng_serein_details_date;
	case Label::Edited: return tr::lng_serein_details_edited;
	case Label::PostAuthor: return tr::lng_serein_inspector_post_author;
	case Label::ViaBot: return tr::lng_serein_inspector_via_bot;
	case Label::Album: return tr::lng_serein_inspector_album;
	case Label::Timer: return tr::lng_serein_inspector_timer;
	case Label::Effect: return tr::lng_serein_inspector_effect;
	case Label::Entities: return tr::lng_serein_inspector_entities;
	case Label::Views: return tr::lng_serein_details_views;
	case Label::Forwards: return tr::lng_serein_details_forwards;
	case Label::Replies: return tr::lng_serein_inspector_replies;
	case Label::Reactions: return tr::lng_serein_inspector_reactions;
	case Label::ReplyTo: return tr::lng_serein_inspector_reply_to;
	case Label::ReplyChat: return tr::lng_serein_inspector_reply_chat;
	case Label::Topic: return tr::lng_serein_inspector_topic;
	case Label::Quote: return tr::lng_serein_inspector_quote;
	case Label::ForwardFrom: return tr::lng_serein_details_forwarded_from;
	case Label::ForwardName: return tr::lng_serein_inspector_forward_name;
	case Label::ForwardDate: return tr::lng_serein_details_forwarded_date;
	case Label::ForwardPost: return tr::lng_serein_inspector_forward_post;
	case Label::ForwardSignature: return tr::lng_serein_inspector_signature;
	case Label::SavedFrom: return tr::lng_serein_inspector_saved_from;
	case Label::MediaType: return tr::lng_serein_inspector_media_type;
	case Label::PhotoId: return tr::lng_serein_inspector_photo_id;
	case Label::DocumentId: return tr::lng_serein_inspector_document_id;
	case Label::FileName: return tr::lng_serein_inspector_file_name;
	case Label::FileSize: return tr::lng_serein_inspector_file_size;
	case Label::MimeType: return tr::lng_serein_inspector_mime_type;
	case Label::DataCenter: return tr::lng_serein_inspector_data_center;
	case Label::Dimensions: return tr::lng_serein_inspector_dimensions;
	case Label::Duration: return tr::lng_serein_inspector_duration;
	case Label::Title: return tr::lng_serein_inspector_title;
	case Label::Performer: return tr::lng_serein_inspector_performer;
	case Label::Emoji: return tr::lng_serein_inspector_emoji;
	case Label::PeerId: return tr::lng_serein_inspector_peer_id;
	case Label::About: return tr::lng_serein_inspector_about;
	case Label::Members: return tr::lng_serein_inspector_members;
	case Label::Admins: return tr::lng_serein_inspector_admins;
	case Label::Online: return tr::lng_serein_inspector_online;
	case Label::CommonChats: return tr::lng_serein_inspector_common_chats;
	case Label::Linked: return tr::lng_serein_inspector_linked;
	case Label::SlowMode: return tr::lng_serein_inspector_slow_mode;
	case Label::Username: return tr::lng_serein_inspector_username;
	case Label::Phone: return tr::lng_serein_inspector_phone;
	case Label::Pinned: return tr::lng_serein_inspector_pinned;
	}
	Unexpected("Label in Inspector::LabelText.");
}

[[nodiscard]] const style::icon &LabelIcon(Label label) {
	switch (label) {
	case Label::MessageId:
	case Label::MimeType:
	case Label::PeerId: return st::menuIconInfo;
	case Label::Chat:
	case Label::ReplyChat:
	case Label::CommonChats: return st::menuIconChats;
	case Label::Sender:
	case Label::Performer:
	case Label::Online: return st::menuIconProfile;
	case Label::Date:
	case Label::ForwardDate: return st::menuIconSchedule;
	case Label::Edited: return st::menuIconEdit;
	case Label::PostAuthor:
	case Label::ForwardSignature: return st::menuIconSigned;
	case Label::ViaBot: return st::menuIconBot;
	case Label::Album: return st::menuIconPhotoSet;
	case Label::Timer:
	case Label::SlowMode: return st::menuIconTimer;
	case Label::Effect: return st::menuIconPremium;
	case Label::Entities: return st::menuIconFont;
	case Label::Views: return st::menuIconStats;
	case Label::Forwards:
	case Label::ForwardFrom: return st::menuIconForward;
	case Label::Replies: return st::menuIconViewReplies;
	case Label::Reactions: return st::menuIconReactions;
	case Label::ReplyTo: return st::menuIconReply;
	case Label::Topic: return st::menuIconTopics;
	case Label::Quote: return st::menuIconChatBubble;
	case Label::ForwardName: return st::menuIconUserHide;
	case Label::ForwardPost:
	case Label::Linked: return st::menuIconChannel;
	case Label::SavedFrom: return st::menuIconSavedMessages;
	case Label::MediaType:
	case Label::DocumentId:
	case Label::FileName: return st::menuIconFile;
	case Label::PhotoId:
	case Label::Dimensions: return st::menuIconPhoto;
	case Label::FileSize: return st::menuIconStorage;
	case Label::DataCenter: return st::menuIconIpAddress;
	case Label::Duration: return st::menuIconHourglass;
	case Label::Title: return st::menuIconSoundOn;
	case Label::Emoji: return st::menuIconEmoji;
	case Label::About: return st::menuIconArticle;
	case Label::Members: return st::menuIconGroups;
	case Label::Admins: return st::menuIconAdmin;
	case Label::Username: return st::menuIconUsername;
	case Label::Phone: return st::menuIconPhone;
	case Label::Pinned: return st::menuIconPin;
	}
	Unexpected("Label in Inspector::LabelIcon.");
}

[[nodiscard]] Fact PeerFact(Section section, Label label, PeerId id) {
	auto result = Fact{
		.section = section,
		.label = label,
		.format = Format::Peer,
	};
	if (peerIsUser(id)) {
		result.value = QString::number(peerToUser(id).bare);
		result.extra = u"user"_q;
	} else if (peerIsChat(id)) {
		result.value = QString::number(peerToChat(id).bare);
		result.extra = u"chat"_q;
	} else {
		result.value = QString::number(peerToChannel(id).bare);
		result.extra = u"channel"_q;
	}
	return result;
}

[[nodiscard]] std::vector<Fact> LocalFacts(gsl::not_null<HistoryItem*> item) {
	const auto peer = item->history()->peer;
	auto result = std::vector<Fact>{
		{
			.label = Label::MessageId,
			.format = Format::Number,
			.value = QString::number(item->id.bare),
		},
		PeerFact(Section::Overview, Label::Chat, peer->id),
	};
	if (const auto from = item->from(); from != peer) {
		result.push_back(PeerFact(Section::Overview, Label::Sender, from->id));
	}
	result.push_back({
		.label = Label::Date,
		.format = Format::Date,
		.value = QString::number(item->date()),
	});
	return result;
}

[[nodiscard]] PeerId FactPeer(const Fact &fact) {
	const auto id = fact.value.toLongLong();
	if (fact.format == Format::User || fact.extra == u"user"_q) {
		return peerFromUser(UserId(id));
	} else if (fact.extra == u"chat"_q) {
		return peerFromChat(ChatId(id));
	}
	return peerFromChannel(ChannelId(id));
}

[[nodiscard]] QString BotApiId(PeerId id) {
	if (peerIsUser(id)) {
		return QString::number(peerToUser(id).bare);
	} else if (peerIsChat(id)) {
		return u"-"_q + QString::number(peerToChat(id).bare);
	}
	return u"-100"_q + QString::number(peerToChannel(id).bare);
}

[[nodiscard]] QString KindText(const QString &type) {
	constexpr auto kPrefix = QStringView(u"messageMedia");
	return type.startsWith(kPrefix) ? type.mid(kPrefix.size()) : type;
}

[[nodiscard]] Shown Present(
		gsl::not_null<Main::Session*> session,
		const Fact &fact) {
	switch (fact.format) {
	case Format::Date: {
		const auto text = QLocale().toString(
			base::unixtime::parse(TimeId(fact.value.toLongLong())),
			u"yyyy-MM-dd HH:mm:ss"_q);
		return { text, text };
	}
	case Format::Size:
		return { Ui::FormatSizeText(fact.value.toLongLong()), fact.value };
	case Format::Duration: {
		const auto seconds = fact.value.toDouble();
		return {
			((fact.label == Label::Duration)
				? Ui::FormatDurationText(qint64(seconds))
				: Ui::FormatTTL(seconds)),
			fact.value,
		};
	}
	case Format::Peer:
	case Format::User: {
		const auto id = FactPeer(fact);
		const auto number = BotApiId(id);
		const auto peer = session->data().peerLoaded(id);
		return { peer ? (peer->name() + u" · "_q + number) : number, number };
	}
	case Format::Kind:
		return { KindText(fact.value), fact.value };
	case Format::Number:
		if (!fact.extra.isEmpty()) {
			const auto text = fact.value + u" · "_q + fact.extra;
			return { text, text };
		}
		break;
	case Format::Text:
	case Format::Dimensions:
		break;
	}
	return { fact.value, fact.value };
}

void AddRow(
		gsl::not_null<Ui::GenericBox*> box,
		gsl::not_null<Ui::VerticalLayout*> container,
		const Shown &shown,
		const QString &label,
		const style::icon &icon,
		const style::color &tile) {
	const auto button = container->add(object_ptr<Ui::SettingsButton>(
		container,
		rpl::single(shown.text),
		st::sereinSettingsButtonDescribed));
	Display::AddIconTile(button, icon, tile);
	container->add(
		object_ptr<Ui::FlatLabel>(container, label, st::sereinSettingsAbout),
		st::sereinSettingsAboutPadding);
	const auto copy = shown.copy;
	button->setClickedCallback([=] {
		TextUtilities::SetClipboardText(TextForMimeData::Simple(copy));
		box->showToast(tr::lng_text_copied(tr::now));
	});
}

void AddTitle(
		gsl::not_null<Ui::VerticalLayout*> container,
		rpl::producer<QString> title,
		bool first) {
	if (!first) {
		Ui::AddSkip(container);
		Ui::AddDivider(container);
	}
	Ui::AddSkip(container);
	Ui::AddSubsectionTitle(container, std::move(title));
}

void AddFlags(
		gsl::not_null<Ui::VerticalLayout*> container,
		const Node &root) {
	auto names = QStringList();
	for (const auto &flag : RaisedFlags(root)) {
		names.push_back(Humanize(flag));
	}
	if (names.isEmpty()) {
		return;
	}
	AddTitle(container, tr::lng_serein_inspector_flags(), false);
	const auto label = container->add(
		object_ptr<Ui::FlatLabel>(
			container,
			names.join(u" · "_q),
			st::boxLabel),
		st::sereinInspectorTextPadding);
	label->setSelectable(true);
}

void AddRaw(
		gsl::not_null<Ui::VerticalLayout*> container,
		const Node &root) {
	AddTitle(container, tr::lng_serein_inspector_raw(), false);
	const auto toggle = container->add(object_ptr<Ui::SettingsButton>(
		container,
		tr::lng_serein_inspector_raw_show(),
		st::sereinSettingsButton));
	Display::AddIconTile(toggle, st::menuIconExpand, st::settingsIconBg8);
	const auto wrap = container->add(
		object_ptr<Ui::SlideWrap<Ui::VerticalLayout>>(
			container,
			object_ptr<Ui::VerticalLayout>(container)));
	const auto inner = wrap->entity();
	const auto text = RenderText(root);
	const auto label = inner->add(
		object_ptr<Ui::FlatLabel>(
			inner,
			rpl::single(TextWithEntities{
				text,
				{ EntityInText(EntityType::Pre, 0, int(text.size())) },
			}),
			st::sereinInspectorRaw),
		st::sereinInspectorTextPadding);
	label->setSelectable(true);
	wrap->hide(anim::type::instant);
	toggle->setClickedCallback([=] {
		wrap->toggle(!wrap->toggled(), anim::type::normal);
	});
}

[[nodiscard]] QString OriginText(const Snapshot &snapshot) {
	switch (snapshot.origin) {
	case Origin::Server: return tr::lng_serein_inspector_from_server(tr::now);
	case Origin::Saved: return tr::lng_serein_inspector_from_saved(tr::now);
	case Origin::Missing: return tr::lng_serein_inspector_missing(tr::now);
	case Origin::Failed: break;
	}
	return tr::lng_serein_inspector_failed(
		tr::now,
		lt_error,
		snapshot.error.isEmpty() ? u"-"_q : snapshot.error);
}

void Fill(
		gsl::not_null<Ui::GenericBox*> box,
		gsl::not_null<Main::Session*> session,
		const Snapshot &snapshot,
		const std::vector<Fact> &local,
		const ExtraFacts &extra) {
	const auto container = box->verticalLayout();
	Ui::AddDividerText(
		container,
		rpl::single(TextWithEntities{ OriginText(snapshot) }));
	const auto facts = snapshot.root ? CollectFacts(*snapshot.root) : local;
	auto first = true;
	for (const auto section : kSections) {
		auto titled = false;
		for (const auto &fact : facts) {
			if (fact.section != section) {
				continue;
			} else if (!titled) {
				AddTitle(container, SectionTitle(section)(), first);
				titled = true;
				first = false;
			}
			AddRow(
				box,
				container,
				Present(session, fact),
				LabelText(fact.label)(tr::now),
				LabelIcon(fact.label),
				SectionTile(section));
		}
	}
	if (!extra.empty()) {
		AddTitle(container, tr::lng_serein_inspector_more(), first);
		first = false;
		for (const auto &[label, value] : extra) {
			AddRow(
				box,
				container,
				{ value, value },
				label,
				st::menuIconStickers,
				st::settingsIconBg8);
		}
	}
	if (snapshot.root) {
		AddFlags(container, *snapshot.root);
		AddRaw(container, *snapshot.root);
	}
	Ui::AddSkip(container);
}

void DetailsBox(
		gsl::not_null<Ui::GenericBox*> box,
		gsl::not_null<Main::Session*> session,
		Loader load,
		std::vector<Fact> local,
		ExtraFacts extra) {
	box->setWidth(st::sereinInspectorWidth);
	const auto api = box->lifetime().make_state<MTP::Sender>(&session->mtp());
	const auto json = box->lifetime().make_state<QByteArray>();
	const auto container = box->verticalLayout();
	const auto loading = container->add(
		object_ptr<Ui::FlatLabel>(
			container,
			tr::lng_serein_inspector_loading(),
			st::boxLabel),
		st::boxRowPadding);
	box->addLeftButton(tr::lng_serein_inspector_copy_json(), [=] {
		if (json->isEmpty()) {
			box->showToast(tr::lng_serein_inspector_no_data(tr::now));
			return;
		}
		TextUtilities::SetClipboardText(
			TextForMimeData::Simple(QString::fromUtf8(*json)));
		box->showToast(tr::lng_text_copied(tr::now));
	});
	box->addButton(tr::lng_close(), [=] { box->closeBox(); });
	load(*api, [=](Snapshot snapshot) {
		delete loading;
		if (snapshot.root) {
			*json = RenderJson(*snapshot.root);
		}
		Fill(box, session, snapshot, local, extra);
	});
}

} // namespace

void ShowMessageDetails(
		gsl::not_null<Window::SessionController*> controller,
		gsl::not_null<HistoryItem*> item) {
	const auto session = &controller->session();
	const auto id = item->fullId();
	const auto local = LocalFacts(item);
	const auto extra = CollectExtraFacts(item);
	controller->show(Box([=](not_null<Ui::GenericBox*> box) {
		box->setTitle(tr::lng_serein_menu_details());
		DetailsBox(box, session, [=](
				MTP::Sender &api,
				Fn<void(Snapshot)> done) {
			if (const auto current = session->data().message(id)) {
				LoadMessage(current, api, std::move(done));
			} else {
				done({});
			}
		}, local, extra);
	}));
}

void ShowPeerDetails(
		gsl::not_null<Window::SessionController*> controller,
		gsl::not_null<PeerData*> peer) {
	const auto session = &controller->session();
	const auto local = std::vector<Fact>{
		PeerFact(Section::Chat, Label::PeerId, peer->id),
	};
	controller->show(Box([=](not_null<Ui::GenericBox*> box) {
		box->setTitle(tr::lng_serein_inspector_chat_title());
		DetailsBox(box, session, [=](
				MTP::Sender &api,
				Fn<void(Snapshot)> done) {
			LoadPeer(peer, api, std::move(done));
		}, local, {});
	}));
}

} // namespace Serein::Inspector
