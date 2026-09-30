#pragma once

class QString;
class QVariant;

namespace Serein::Links {

[[nodiscard]] bool HandleExternalLink(
	const QString &url,
	const QVariant &context);

} // namespace Serein::Links
