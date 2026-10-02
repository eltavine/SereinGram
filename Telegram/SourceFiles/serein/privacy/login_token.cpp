#include "serein/privacy/login_token.h"

#include "base/basic_types.h"

#include <QtCore/QUrl>
#include <QtCore/QUrlQuery>

namespace Serein::Privacy {
namespace {

constexpr auto kMaxTokenSize = 1024;

} // namespace

std::optional<QByteArray> ParseLoginToken(const QString &text) {
	const auto url = QUrl(text.trimmed(), QUrl::StrictMode);
	if (!url.isValid()
		|| url.scheme() != u"tg"_q
		|| url.host() != u"login"_q) {
		return std::nullopt;
	}
	const auto value = QUrlQuery(url).queryItemValue(
		u"token"_q,
		QUrl::FullyDecoded);
	const auto decoded = QByteArray::fromBase64Encoding(
		value.toLatin1(),
		QByteArray::Base64UrlEncoding
			| QByteArray::AbortOnBase64DecodingErrors);
	if (!decoded
		|| decoded.decoded.isEmpty()
		|| decoded.decoded.size() > kMaxTokenSize) {
		return std::nullopt;
	}
	return decoded.decoded;
}

} // namespace Serein::Privacy
