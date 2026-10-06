#pragma once

#include "serein/features/inspector/model/tl_tree.h"

#include <QtCore/QStringList>

#include <vector>

namespace Serein::Inspector {

enum class Section {
	Overview,
	Reply,
	Forward,
	Media,
	Counters,
	Chat,
};

enum class Format {
	Text,
	Number,
	Date,
	Size,
	Duration,
	Peer,
	User,
	Kind,
	Dimensions,
};

enum class Label {
	MessageId,
	Chat,
	Sender,
	Date,
	Edited,
	PostAuthor,
	ViaBot,
	Album,
	Timer,
	Effect,
	Entities,
	Views,
	Forwards,
	Replies,
	Reactions,
	ReplyTo,
	ReplyChat,
	Topic,
	Quote,
	ForwardFrom,
	ForwardName,
	ForwardDate,
	ForwardPost,
	ForwardSignature,
	SavedFrom,
	MediaType,
	PhotoId,
	DocumentId,
	FileName,
	FileSize,
	MimeType,
	DataCenter,
	Dimensions,
	Duration,
	Title,
	Performer,
	Emoji,
	PeerId,
	About,
	Members,
	Admins,
	Online,
	CommonChats,
	Linked,
	SlowMode,
	Username,
	Phone,
	Pinned,
};

struct Fact {
	Section section = Section::Overview;
	Label label = Label::MessageId;
	Format format = Format::Text;
	QString value;
	QString extra;
};

[[nodiscard]] std::vector<Fact> CollectFacts(const Node &root);
[[nodiscard]] QStringList RaisedFlags(const Node &object);

} // namespace Serein::Inspector
