#include "serein/links/model.h"
#include "base/basic_types.h"

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
			|| QUuid(rule.id).isNull()
			|| (rule.removeParameters.empty() && rule.replacementHost.isEmpty())) {
			return false;
		}
		ids.insert(rule.id);
	}
	return true;
}

bool Validate(const QByteArray &raw) {
	return raw.isEmpty() || ParseLinkRules(raw).has_value();
}

std::optional<LinkRules> ReadRules(const QByteArray &raw) {
	return raw.isEmpty() ? std::make_optional(LinkRules()) : ParseLinkRules(raw);
}

QByteArray WriteRules(const LinkRules &rules) {
	return (rules == LinkRules()) ? QByteArray() : SerializeLinkRules(rules);
}

LinkRule NewRule() {
	auto rule = LinkRule();
	rule.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
	return rule;
}

Result Rewrite(const QByteArray &raw, const QString &original) {
	if (const auto rules = ReadRules(raw)) {
		return Rewrite(*rules, original);
	}
	auto result = Result();
	result.error = u"invalid link rule configuration"_q;
	return result;
}

Result Rewrite(const LinkRules &rules, const QString &original) {
	auto result = Result();
	if (original.size() > 16384) {
		result.error = u"invalid external URL"_q;
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
	for (const auto &rule : rules.rules) {
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

QString RewritePreviewLink(const QByteArray &raw, const QString &link) {
	const auto absolute = link.contains(u"://"_q)
		? link
		: (u"https://"_q + link);
	const auto result = Rewrite(raw, absolute);
	return (result.error.isEmpty() && result.changed)
		? result.url.toString(QUrl::FullyEncoded)
		: link;
}

} // namespace Serein::Links
