#include "nagram/chats/layout.h"

#include "nagram/chats/options.h"
#include "styles/style_dialogs.h"

#include <algorithm>

namespace Nagram::Chats {

const style::DialogRow &RowStyle(bool hasTags, bool wideRow) {
	if (wideRow) {
		return hasTags ? st::taggedForumDialogRow : st::forumDialogRow;
	} else if (hasTags) {
		return st::taggedDialogRow;
	}
	const auto compact = ForDevice().Get(kCompactList);
	switch (ForDevice().Get(kPreviewLines)) {
	case 2: return compact ? st::compactTwoLineDialogRow : st::twoLineDialogRow;
	case 3: return compact ? st::compactThreeLineDialogRow : st::threeLineDialogRow;
	default: return compact ? st::compactDialogRow : st::defaultDialogRow;
	}
}

int PreviewLines() {
	return std::max(ForDevice().Get(kPreviewLines), 1);
}

bool HideSavedAndArchivedPreviews() {
	return ForDevice().Get(kHideSavedAndArchivedPreviews);
}

bool HidePreview(bool folder, bool savedMessages) {
	return (folder || savedMessages) && HideSavedAndArchivedPreviews();
}

bool HideStories() {
	return ForDevice().Get(kHideStories);
}

} // namespace Nagram::Chats
