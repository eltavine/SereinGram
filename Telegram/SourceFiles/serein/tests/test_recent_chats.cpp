#include "serein/chats/recent.h"
#include "serein/chats/options.h"
#include "serein/tests/require.h"

#include <doctest/doctest.h>

TEST_CASE("RecentChats") {
	using namespace Serein::Chats;
	auto list = QString();
	list = PushRecentChat(list, 7);
	list = PushRecentChat(list, 9);
	list = PushRecentChat(list, 7);
	Require(list == QString::fromLatin1("7,9"), "recent chat not moved to front");
	Require(ParseRecentChats(list) == std::vector<quint64>{ 7, 9 },
		"recent chats not parsed");
	for (auto id = quint64(100); id != 140; ++id) {
		list = PushRecentChat(list, id);
	}
	const auto ids = ParseRecentChats(list);
	Require(int(ids.size()) == kRecentChatsLimit && ids.front() == 139,
		"recent chats not capped");
	Require(ValidRecentChats(QString()) && ValidRecentChats(list),
		"valid recent chats rejected");
	for (const auto bad : { "7,7", "0", "-1", "07", "7,", "a" }) {
		Require(!ValidRecentChats(QString::fromLatin1(bad)),
			"malformed recent chats accepted");
	}
}
