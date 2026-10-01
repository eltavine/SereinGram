#include "serein/hooks/links/preview.h"

#include "serein/links/model.h"

namespace Serein::Hooks::Links {

QString PreviewLink(const QString &link) {
	if (!ForDevice().Get(Serein::Links::kPreviewLinkRules)) {
		return link;
	}
	const auto absolute = link.contains(u"://"_q)
		? link
		: (u"https://"_q + link);
	const auto result = Serein::Links::Rewrite(
		ForDevice().Get(Serein::Links::kRules),
		absolute);
	return (result.error.isEmpty() && result.changed)
		? result.url.toString(QUrl::FullyEncoded)
		: link;
}

} // namespace Serein::Hooks::Links
