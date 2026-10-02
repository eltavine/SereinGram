#include "serein/hooks/links/preview.h"

#include "serein/links/model.h"

namespace Serein::Hooks::Links {

QString PreviewLink(const QString &link) {
	return ForDevice().Get(Serein::Links::kPreviewLinkRules)
		? Serein::Links::RewritePreviewLink(
			ForDevice().Get(Serein::Links::kRules),
			link)
		: link;
}

} // namespace Serein::Hooks::Links
