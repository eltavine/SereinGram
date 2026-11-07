#include "serein/features/inspector/model/facts.h"

#include "base/basic_types.h"

#include <array>

namespace Serein::Inspector {
namespace {

struct Rule {
	const char *root = nullptr;
	const char *path = nullptr;
	Section section = Section::Overview;
	Label label = Label::MessageId;
	Format format = Format::Text;
};

struct OwnerRule {
	const char *path = nullptr;
	Label label = Label::MessageId;
	Format format = Format::Text;
};

constexpr auto kMessage = "message";
constexpr auto kUserFull = "users_userFull";
constexpr auto kChatFull = "messages_chatFull";

constexpr auto kRules = std::array{
	Rule{ kMessage, "id", Section::Overview, Label::MessageId, Format::Number },
	Rule{ kMessage, "peer_id", Section::Overview, Label::Chat, Format::Peer },
	Rule{ kMessage, "from_id", Section::Overview, Label::Sender, Format::Peer },
	Rule{ kMessage, "date", Section::Overview, Label::Date, Format::Date },
	Rule{ kMessage, "edit_date", Section::Overview, Label::Edited, Format::Date },
	Rule{ kMessage, "post_author", Section::Overview, Label::PostAuthor },
	Rule{ kMessage, "via_bot_id", Section::Overview, Label::ViaBot, Format::User },
	Rule{ kMessage, "grouped_id", Section::Overview, Label::Album, Format::Number },
	Rule{ kMessage, "ttl_period", Section::Overview, Label::Timer, Format::Duration },
	Rule{ kMessage, "effect", Section::Overview, Label::Effect, Format::Number },
	Rule{ kMessage, "reply_to.reply_to_msg_id", Section::Reply, Label::ReplyTo, Format::Number },
	Rule{ kMessage, "reply_to.reply_to_peer_id", Section::Reply, Label::ReplyChat, Format::Peer },
	Rule{ kMessage, "reply_to.reply_to_top_id", Section::Reply, Label::Topic, Format::Number },
	Rule{ kMessage, "reply_to.quote_text", Section::Reply, Label::Quote },
	Rule{ kMessage, "fwd_from.from_id", Section::Forward, Label::ForwardFrom, Format::Peer },
	Rule{ kMessage, "fwd_from.from_name", Section::Forward, Label::ForwardName },
	Rule{ kMessage, "fwd_from.date", Section::Forward, Label::ForwardDate, Format::Date },
	Rule{ kMessage, "fwd_from.channel_post", Section::Forward, Label::ForwardPost, Format::Number },
	Rule{ kMessage, "fwd_from.post_author", Section::Forward, Label::ForwardSignature },
	Rule{ kMessage, "fwd_from.saved_from_peer", Section::Forward, Label::SavedFrom, Format::Peer },
	Rule{ kMessage, "media", Section::Media, Label::MediaType, Format::Kind },
	Rule{ kMessage, "media.photo.id", Section::Media, Label::PhotoId, Format::Number },
	Rule{ kMessage, "media.photo.dc_id", Section::Media, Label::DataCenter, Format::Number },
	Rule{ kMessage, "media.document.id", Section::Media, Label::DocumentId, Format::Number },
	Rule{ kMessage, "media.document.attributes.*.file_name", Section::Media, Label::FileName },
	Rule{ kMessage, "media.document.mime_type", Section::Media, Label::MimeType },
	Rule{ kMessage, "media.document.size", Section::Media, Label::FileSize, Format::Size },
	Rule{ kMessage, "media.document.dc_id", Section::Media, Label::DataCenter, Format::Number },
	Rule{ kMessage, "media.document.attributes.*.duration", Section::Media, Label::Duration, Format::Duration },
	Rule{ kMessage, "media.document.attributes.*.title", Section::Media, Label::Title },
	Rule{ kMessage, "media.document.attributes.*.performer", Section::Media, Label::Performer },
	Rule{ kMessage, "media.document.attributes.*.alt", Section::Media, Label::Emoji },
	Rule{ kMessage, "views", Section::Counters, Label::Views, Format::Number },
	Rule{ kMessage, "forwards", Section::Counters, Label::Forwards, Format::Number },
	Rule{ kMessage, "replies.replies", Section::Counters, Label::Replies, Format::Number },
	Rule{ kUserFull, "full_user.id", Section::Chat, Label::PeerId, Format::Number },
	Rule{ kUserFull, "full_user.about", Section::Chat, Label::About },
	Rule{ kUserFull, "full_user.common_chats_count", Section::Chat, Label::CommonChats, Format::Number },
	Rule{ kUserFull, "full_user.pinned_msg_id", Section::Chat, Label::Pinned, Format::Number },
	Rule{ kUserFull, "full_user.ttl_period", Section::Chat, Label::Timer, Format::Duration },
	Rule{ kChatFull, "full_chat.id", Section::Chat, Label::PeerId, Format::Number },
	Rule{ kChatFull, "full_chat.about", Section::Chat, Label::About },
	Rule{ kChatFull, "full_chat.participants_count", Section::Chat, Label::Members, Format::Number },
	Rule{ kChatFull, "full_chat.admins_count", Section::Chat, Label::Admins, Format::Number },
	Rule{ kChatFull, "full_chat.online_count", Section::Chat, Label::Online, Format::Number },
	Rule{ kChatFull, "full_chat.linked_chat_id", Section::Chat, Label::Linked, Format::Number },
	Rule{ kChatFull, "full_chat.slowmode_seconds", Section::Chat, Label::SlowMode, Format::Duration },
	Rule{ kChatFull, "full_chat.ttl_period", Section::Chat, Label::Timer, Format::Duration },
	Rule{ kChatFull, "full_chat.pinned_msg_id", Section::Chat, Label::Pinned, Format::Number },
};

constexpr auto kOwnerRules = std::array{
	OwnerRule{ "username", Label::Username },
	OwnerRule{ "phone", Label::Phone },
	OwnerRule{ "date", Label::Date, Format::Date },
	OwnerRule{ "photo.dc_id", Label::DataCenter, Format::Number },
};

void Walk(
		const Node &node,
		const QStringList &path,
		int index,
		std::vector<const Node*> &found) {
	if (index == path.size()) {
		found.push_back(&node);
		return;
	}
	const auto &segment = path[index];
	for (const auto &child : node.children) {
		if (segment == u"*"_q || child.key == segment) {
			Walk(child, path, index + 1, found);
		}
	}
}

[[nodiscard]] std::vector<const Node*> Find(
		const Node &node,
		const char *path) {
	auto result = std::vector<const Node*>();
	Walk(node, QString::fromLatin1(path).split(u'.'), 0, result);
	return result;
}

[[nodiscard]] bool FillPeer(const Node &node, Fact &fact) {
	const auto pick = [&](QStringView field, const QString &kind) {
		if (const auto id = node.find(field)) {
			fact.value = id->value;
			fact.extra = kind;
			return true;
		}
		return false;
	};
	return pick(u"user_id", u"user"_q)
		|| pick(u"chat_id", u"chat"_q)
		|| pick(u"channel_id", u"channel"_q);
}

[[nodiscard]] bool Fill(const Node &node, Fact &fact) {
	switch (fact.format) {
	case Format::Peer: return FillPeer(node, fact);
	case Format::Kind: fact.value = node.type; return !fact.value.isEmpty();
	default: break;
	}
	if (node.kind == NodeKind::Object || node.kind == NodeKind::Vector) {
		return false;
	}
	fact.value = node.value;
	return !fact.value.isEmpty();
}

void AddFact(
		const Node &node,
		Section section,
		Label label,
		Format format,
		std::vector<Fact> &result) {
	auto fact = Fact{ .section = section, .label = label, .format = format };
	if (Fill(node, fact)) {
		result.push_back(std::move(fact));
	}
}

void AddDimensions(const Node &root, std::vector<Fact> &result) {
	auto nodes = Find(root, "media.document.attributes.*");
	const auto sizes = Find(root, "media.photo.sizes.*");
	nodes.insert(end(nodes), begin(sizes), end(sizes));
	auto best = QString();
	auto area = qint64(-1);
	for (const auto node : nodes) {
		const auto w = node->find(u"w");
		const auto h = node->find(u"h");
		if (!w || !h) {
			continue;
		}
		const auto current = w->value.toLongLong() * h->value.toLongLong();
		if (current > area) {
			area = current;
			best = w->value + QChar(0x00D7) + h->value;
		}
	}
	if (!best.isEmpty()) {
		result.push_back({
			.section = Section::Media,
			.label = Label::Dimensions,
			.format = Format::Dimensions,
			.value = best,
		});
	}
}

void AddEntities(const Node &root, std::vector<Fact> &result) {
	const auto entities = root.find(u"entities");
	if (!entities || entities->children.empty()) {
		return;
	}
	constexpr auto kPrefix = QStringView(u"messageEntity");
	auto kinds = QStringList();
	for (const auto &entity : entities->children) {
		const auto kind = entity.type.startsWith(kPrefix)
			? entity.type.mid(kPrefix.size())
			: entity.type;
		if (!kinds.contains(kind)) {
			kinds.push_back(kind);
		}
	}
	result.push_back({
		.section = Section::Overview,
		.label = Label::Entities,
		.format = Format::Number,
		.value = QString::number(entities->children.size()),
		.extra = kinds.join(u", "_q),
	});
}

void AddReactions(const Node &root, std::vector<Fact> &result) {
	const auto counts = Find(root, "reactions.results.*.count");
	if (counts.empty()) {
		return;
	}
	auto total = qint64();
	for (const auto count : counts) {
		total += count->value.toLongLong();
	}
	result.push_back({
		.section = Section::Counters,
		.label = Label::Reactions,
		.format = Format::Number,
		.value = QString::number(total),
	});
}

[[nodiscard]] const Node *Owner(const Node &root) {
	const auto full = root.find(u"full_user");
	const auto info = full ? full : root.find(u"full_chat");
	const auto id = info ? info->find(u"id") : nullptr;
	const auto list = root.find(full ? u"users" : u"chats");
	if (!id || !list) {
		return nullptr;
	}
	for (const auto &entry : list->children) {
		if (const auto own = entry.find(u"id"); own && own->value == id->value) {
			return &entry;
		}
	}
	return nullptr;
}

void AddOwner(const Node &root, std::vector<Fact> &result) {
	const auto owner = Owner(root);
	if (!owner) {
		return;
	}
	for (const auto &rule : kOwnerRules) {
		for (const auto node : Find(*owner, rule.path)) {
			AddFact(*node, Section::Chat, rule.label, rule.format, result);
		}
	}
}

} // namespace

std::vector<Fact> CollectFacts(const Node &root) {
	auto result = std::vector<Fact>();
	for (const auto &rule : kRules) {
		if (root.type != QLatin1String(rule.root)) {
			continue;
		}
		for (const auto node : Find(root, rule.path)) {
			AddFact(*node, rule.section, rule.label, rule.format, result);
		}
	}
	if (root.type == QLatin1String(kMessage)) {
		AddDimensions(root, result);
		AddEntities(root, result);
		AddReactions(root, result);
	} else {
		AddOwner(root, result);
	}
	return result;
}

QStringList RaisedFlags(const Node &object) {
	auto result = QStringList();
	for (const auto &child : object.children) {
		if (child.kind == NodeKind::Flag) {
			result.push_back(child.key);
		}
	}
	return result;
}

} // namespace Serein::Inspector
