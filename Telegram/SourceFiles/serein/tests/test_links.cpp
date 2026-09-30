#include "serein/links/model.h"
#include "base/basic_types.h"

#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>

#include <iostream>
#include <stdexcept>

namespace {

void Require(bool value, const char *message) {
	if (!value) {
		throw std::runtime_error(message);
	}
}

} // namespace

void TestLinks() {
	using namespace Serein::Links;
	const auto newRule = NewRule(u"example.com"_q,
		u"mirror.example"_q, { u"utm_*"_q });
	Require(!newRule.value(u"enabled"_q).toBool(),
		"new link rule is enabled by default");
	auto config = Defaults();
	config.insert(u"rules"_q, QJsonArray{ newRule });
	auto raw = QJsonDocument(config).toJson(QJsonDocument::Compact);
	Require(Validate(raw), "valid link rule rejected");
	const auto original = u"http://example.com/p?utm_source=x&keep=y#f"_q;
	Require(!Rewrite(raw, original).changed,
		"disabled new rule changed link");
	auto enabled = newRule;
	enabled.insert(u"enabled"_q, true);
	config.insert(u"rules"_q, QJsonArray{ enabled });
	raw = QJsonDocument(config).toJson(QJsonDocument::Compact);
	const auto changed = Rewrite(raw, original);
	Require(changed.error.isEmpty() && changed.changed
		&& changed.url.toString(QUrl::FullyEncoded)
			== u"https://mirror.example/p?keep=y#f"_q,
		"link rewrite changed the wrong URL components");
	Require(!Rewrite(raw, u"https://other.example/p"_q).changed,
		"rule matched a different host");
	Require(!Rewrite(raw, u"http://name:password@example.com/p"_q).error.isEmpty(),
		"credential-bearing URL was rewritten");
	std::cout << "PASS: Serein link rules" << std::endl;
}
