#include "serein/chats/shown_order.h"

#include "base/basic_types.h"
#include "serein/tests/require.h"

#include <doctest/doctest.h>

TEST_CASE("ShownOrder") {
	using namespace Serein::Chats;
	Require(ParseFolderIds(u"3,1,x,0,-2,7"_q) == base::flat_set<int>{ 1, 3, 7 },
		"folder ids are not parsed");
	const auto ids = std::vector<int>{ 0, 4, 5, 6, 7 };
	const auto none = FolderVisibility();
	Require(none.shown(0) && none.shown(5), "nothing should be hidden");
	Require(MergeShownOrder(ids, { 7, 6, 5, 4, 0 }, none)
		== std::vector<int>{ 7, 6, 5, 4, 0 }, "plain reorder");
	const auto hidden = FolderVisibility{ .hidden = { 5 } };
	Require(MergeShownOrder(ids, { 6, 0, 4, 7 }, hidden)
		== std::vector<int>{ 6, 0, 5, 4, 7 },
		"hidden folder does not keep its place");
	const auto allHidden = FolderVisibility{
		.allChatsHidden = true,
		.hidden = { 6 },
	};
	Require(MergeShownOrder(ids, { 7, 5, 4 }, allHidden)
		== std::vector<int>{ 0, 7, 5, 6, 4 },
		"All Chats and a hidden folder do not keep their places");
	Require(HiddenWithinLimit(ids, 3, allHidden) == 1
		&& HiddenWithinLimit(ids, 5, allHidden) == 2
		&& HiddenWithinLimit(ids, 10, none) == 0,
		"hidden folders inside the limit");
}
