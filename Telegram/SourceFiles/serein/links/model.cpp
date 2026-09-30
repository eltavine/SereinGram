#include "serein/links/model.h"
#include "base/basic_types.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QSet>
#include <QtCore/QUuid>

namespace Serein::Links {
namespace {

bool Removes(const QString &name, const std::vector<QString> &patterns) {
	for (const auto &pattern : patterns) {
		if (pattern.endsWith('*')
			? name.startsWith(pattern.chopped(1)) : name == pattern) {
			return true;
		}
	}
	return false;
}

} // namespace

bool ValidLinkRules(const LinkRules &value) {
	auto ids = QSet<QString>();
	for (const auto &rule : value.rules) {
		if (ids.contains(rule.id)
			|| (rule.removeParameters.empty() && rule.replacementHost.isEmpty())) {
			return false;
		}
		ids.insert(rule.id);
	}
	return true;
}

QJsonObject Defaults() {
	return QJsonDocument::fromJson(SerializeLinkRules(LinkRules())).object();
}

bool Validate(const QByteArray &raw) {
	return raw.isEmpty() || ParseLinkRules(raw).has_value();
}

QJsonObject NewRule(
		const QString &host,
		const QString &replacementHost,
		const QStringList &removeParameters) {
	auto rule = LinkRule();
	rule.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
	rule.host = host;
	rule.replacementHost = replacementHost;
	rule.removeParameters = { removeParameters.begin(), removeParameters.end() };
	return Write(rule).toObject();
}

Result Rewrite(const QByteArray &raw, const QString &original) {
	auto result = Result();
	const auto config = raw.isEmpty()
		? std::make_optional(LinkRules())
		: ParseLinkRules(raw);
	if (!config || original.size() > 16384) {
		result.error = u"invalid link rule configuration"_q;
		return result;
	}
	result.url = QUrl(original, QUrl::StrictMode);
	if (!result.url.isValid() || result.url.host().isEmpty()
		|| (result.url.scheme() != u"https"_q
			&& result.url.scheme() != u"http"_q)) {
		result.error = u"invalid external URL"_q;
		return result;
	}
	const auto originalUrl = result.url;
	const auto host = QString::fromLatin1(QUrl::toAce(result.url.host())).toLower();
	for (const auto &rule : config->rules) {
		if (!rule.enabled || host != rule.host) {
			continue;
		} else if (!result.url.userInfo().isEmpty()) {
			result.error = u"link contains credentials"_q;
			return result;
		}
		if (!rule.replacementHost.isEmpty()
			&& rule.replacementHost != result.url.host()) {
			result.url.setScheme(u"https"_q);
			result.url.setHost(rule.replacementHost);
			result.url.setPort(-1);
		}
		if (!rule.removeParameters.empty() && result.url.hasQuery()) {
			auto kept = QStringList();
			for (const auto &part : result.url.query(
					QUrl::FullyEncoded).split('&')) {
				const auto name = QUrl::fromPercentEncoding(
					part.section('=', 0, 0).toUtf8());
				if (!Removes(name, rule.removeParameters)) {
					kept.push_back(part);
				}
			}
			result.url.setQuery(kept.isEmpty() ? QString() : kept.join('&'),
				QUrl::StrictMode);
		}
		break;
	}
	result.changed = result.url != originalUrl;
	return result;
}

} // namespace Serein::Links
