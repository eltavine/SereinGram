#include "serein/features/inspector/model/facts.h"
#include "serein/features/inspector/model/render.h"
#include "serein/features/inspector/model/tl_tree.h"
#include "base/basic_types.h"
#include "serein/tests/require.h"

#include <doctest/doctest.h>

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include <algorithm>

namespace {

using namespace Serein::Inspector;

const auto kMessageDump = QString::fromUtf8(R"({ core_message
	msg_id: 0 [LONG],
	seq_no: 0 [INT],
	bytes: 120 [INT],
	body: { message
		flags: 16777986 [INT],
		out: YES [ BY BIT 1 IN FIELD flags ],
		pinned: YES [ BY BIT 24 IN FIELD flags ],
		id: 42 [INT],
		from_id: { peerUser
			user_id: 777 [LONG],
		},
		peer_id: { peerChannel
			channel_id: 1234567890 [LONG],
		},
		date: 1700000000 [INT],
		message: "Hello \"world\"\nline" [STRING],
		media: { messageMediaDocument
			flags: 1 [INT],
			document: { document
				flags: 0 [INT],
				id: 555 [LONG],
				access_hash: 1 [LONG],
				file_reference: 01 02 [2 BYTES],
				date: 1700000001 [INT],
				mime_type: "video/mp4" [STRING],
				size: 2048 [LONG],
				dc_id: 4 [INT],
				attributes: [ vector<0x0> (2)
					{ documentAttributeVideo
						flags: 0 [INT],
						duration: 12.5 [DOUBLE],
						w: 1280 [INT],
						h: 720 [INT],
					},
					{ documentAttributeFilename
						file_name: "clip.mp4" [STRING],
					},
				],
			},
		},
		entities: [ vector<0x0> (2)
			{ messageEntityBold
				offset: 0 [INT],
				length: 5 [INT],
			},
			{ messageEntityUrl
				offset: 6 [INT],
				length: 3 [INT],
			},
		],
		views: 100 [INT],
		reactions: { messageReactions
			flags: 0 [INT],
			results: [ vector<0x0> (2)
				{ reactionCount
					flags: 0 [INT],
					reaction: { reactionEmpty },
					count: 3 [INT],
				},
				{ reactionCount
					flags: 0 [INT],
					reaction: { reactionEmpty },
					count: 2 [INT],
				},
			],
		},
		restriction_reason: [ vector<0x0> (0) ],
	},
})");

const auto kUserDump = QString::fromUtf8(R"({ users_userFull
	full_user: { userFull
		flags: 2 [INT],
		id: 9 [LONG],
		about: "bio" [STRING],
		common_chats_count: 2 [INT],
	},
	chats: [ vector<0x0> (0) ],
	users: [ vector<0x0> (2)
		{ user
			flags: 8 [INT],
			id: 10 [LONG],
			username: "bob" [STRING],
		},
		{ user
			flags: 8 [INT],
			id: 9 [LONG],
			username: "alice" [STRING],
			photo: { userProfilePhoto
				flags: 0 [INT],
				photo_id: 3 [LONG],
				dc_id: 2 [INT],
			},
		},
	],
})");

[[nodiscard]] const Node &Child(const Node &node, QStringView key) {
	static const auto kMissing = Node{ .type = u"missing"_q };
	const auto result = node.find(key);
	return result ? *result : kMissing;
}

[[nodiscard]] const Fact *FindFact(
		const std::vector<Fact> &facts,
		Label label) {
	const auto i = std::find_if(begin(facts), end(facts), [&](const Fact &f) {
		return f.label == label;
	});
	return (i != end(facts)) ? &*i : nullptr;
}

[[nodiscard]] QString FactValue(const std::vector<Fact> &facts, Label label) {
	const auto fact = FindFact(facts, label);
	return fact ? fact->value : QString();
}

} // namespace

TEST_CASE("InspectorParsesDump") {
	const auto root = ParseDump(kMessageDump);
	Require(root.has_value(), "dump parsed");
	Require(root->type == u"message"_q, "core_message envelope unwrapped");
	Require(Child(*root, u"id").value == u"42"_q, "number field");
	Require(
		Child(*root, u"message").value == u"Hello \"world\"\nline"_q,
		"escaped string decoded");
	const auto &document = Child(Child(*root, u"media"), u"document");
	Require(document.type == u"document"_q, "nested object");
	Require(
		Child(document, u"file_reference").kind == NodeKind::Bytes,
		"bytes field");
	Require(
		Child(document, u"attributes").children.size() == 2,
		"vector items");
	Require(
		Child(*root, u"restriction_reason").kind == NodeKind::Vector
			&& Child(*root, u"restriction_reason").children.empty(),
		"empty vector");
	Require(
		RaisedFlags(*root) == QStringList{ u"out"_q, u"pinned"_q },
		"raised flags");
}

TEST_CASE("InspectorCollectsMessageFacts") {
	const auto root = ParseDump(kMessageDump);
	Require(root.has_value(), "dump parsed");
	const auto facts = CollectFacts(*root);
	Require(FactValue(facts, Label::MessageId) == u"42"_q, "message id");
	const auto sender = FindFact(facts, Label::Sender);
	Require(
		sender && sender->value == u"777"_q && sender->extra == u"user"_q,
		"sender peer");
	const auto chat = FindFact(facts, Label::Chat);
	Require(chat && chat->extra == u"channel"_q, "chat peer");
	Require(
		FactValue(facts, Label::MediaType) == u"messageMediaDocument"_q,
		"media constructor");
	Require(FactValue(facts, Label::FileName) == u"clip.mp4"_q, "file name");
	Require(FactValue(facts, Label::FileSize) == u"2048"_q, "file size");
	Require(FactValue(facts, Label::DataCenter) == u"4"_q, "data center");
	Require(FactValue(facts, Label::Duration) == u"12.5"_q, "duration");
	Require(
		FactValue(facts, Label::Dimensions)
			== u"1280"_q + QChar(0x00D7) + u"720"_q,
		"largest dimensions");
	const auto entities = FindFact(facts, Label::Entities);
	Require(
		entities
			&& entities->value == u"2"_q
			&& entities->extra == u"Bold, Url"_q,
		"entity count and kinds");
	Require(FactValue(facts, Label::Views) == u"100"_q, "views");
	Require(FactValue(facts, Label::Reactions) == u"5"_q, "reaction total");
	Require(!FindFact(facts, Label::Edited), "absent fields are skipped");
}

TEST_CASE("InspectorCollectsPeerFacts") {
	const auto root = ParseDump(kUserDump);
	Require(root.has_value(), "dump parsed");
	const auto facts = CollectFacts(*root);
	Require(FactValue(facts, Label::PeerId) == u"9"_q, "peer id");
	Require(FactValue(facts, Label::About) == u"bio"_q, "about");
	Require(FactValue(facts, Label::CommonChats) == u"2"_q, "common chats");
	Require(
		FactValue(facts, Label::Username) == u"alice"_q,
		"owner found by id, not by position");
	Require(FactValue(facts, Label::DataCenter) == u"2"_q, "photo data center");
}

TEST_CASE("InspectorRendersTextAndJson") {
	const auto root = ParseDump(kMessageDump);
	Require(root.has_value(), "dump parsed");
	const auto text = RenderText(*root);
	Require(text.startsWith(u"message\n"_q), "constructor first");
	Require(text.contains(u"\n  id: 42\n"_q), "indented field");
	Require(
		text.contains(u"message: \"Hello \\\"world\\\"\\nline\""_q),
		"strings stay on one line");
	const auto json = QJsonDocument::fromJson(RenderJson(*root)).object();
	Require(json.value(u"_"_q).toString() == u"message"_q, "type key");
	Require(json.value(u"id"_q).toInt() == 42, "ints are numbers");
	Require(json.value(u"out"_q).toBool(), "flags are booleans");
	const auto document = json.value(u"media"_q).toObject()
		.value(u"document"_q).toObject();
	Require(
		document.value(u"id"_q).toString() == u"555"_q,
		"longs keep their precision as strings");
	Require(Humanize(u"edit_hide"_q) == u"Edit hide"_q, "humanized flag");
}

TEST_CASE("InspectorParsesSpacedDump") {
	const auto root = ParseDump(u"{ core_message\n  msg_id: 0 [LONG],\n"
		"  seq_no: 0 [INT],\n  bytes: 12 [INT],\n  body: { peerUser\n"
		"    user_id: 5 [LONG],\n  },\n}"_q);
	Require(root && root->type == u"peerUser"_q, "spaces like the dumper");
	Require(
		root && Child(*root, u"user_id").value == u"5"_q,
		"field after spaces");
}

TEST_CASE("InspectorRejectsBrokenDumps") {
	Require(!ParseDump(u"{ message\n  id: 1 [INT],"_q), "unterminated");
	Require(
		!ParseDump(u"[ERROR] (could not decode type)"_q),
		"dumper error");
	Require(!ParseDump(u"{ message\n  id: [INT]\n}"_q), "missing value");
	auto deep = QString();
	for (auto i = 0; i != 80; ++i) {
		deep += u"{ wrap\n  inner: "_q;
	}
	deep += u"1 [INT]"_q;
	for (auto i = 0; i != 80; ++i) {
		deep += u",\n}"_q;
	}
	Require(!ParseDump(deep), "nesting is limited");
}
