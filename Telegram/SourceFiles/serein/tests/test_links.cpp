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

	const auto accepts = [&](QJsonObject object) {
		return Validate(QJsonDocument(object).toJson(QJsonDocument::Compact));
	};
	const auto withRules = [&](QJsonArray rules) {
		auto result = config;
		result.insert(u"rules"_q, rules);
		return result;
	};
	const auto withField = [&](const QString &key, const QJsonValue &value) {
		auto rule = enabled;
		rule.insert(key, value);
		return withRules({ rule });
	};
	auto missing = config;
	missing.remove(u"confirmAll"_q);
	Require(!accepts(missing), "config without confirmAll accepted");
	auto extra = config;
	extra.insert(u"extra"_q, 1);
	Require(!accepts(extra), "config with an unknown key accepted");
	auto partial = enabled;
	partial.remove(u"enabled"_q);
	Require(!accepts(withRules({ partial })), "rule without enabled accepted");
	Require(!accepts(withField(u"host"_q, u"Example.com"_q)), "uppercase host accepted");
	Require(!accepts(withField(u"host"_q, u"example.com\n"_q)), "host with a newline accepted");
	Require(!accepts(withField(u"host"_q, u"a..b"_q)), "host with an empty label accepted");
	Require(!accepts(withField(u"id"_q, enabled.value(u"id"_q).toString().toUpper())),
		"uppercase rule id accepted");
	Require(!accepts(withField(u"removeParameters"_q, QJsonArray{ u"a"_q, u"a"_q })),
		"duplicate parameters accepted");
	Require(!accepts(withField(u"removeParameters"_q, QJsonArray{ u"a b"_q })),
		"invalid parameter accepted");
	auto idle = enabled;
	idle.insert(u"replacementHost"_q, QString());
	idle.insert(u"removeParameters"_q, QJsonArray());
	Require(!accepts(withRules({ idle })), "rule that changes nothing accepted");
	Require(!accepts(withRules({ enabled, enabled })), "duplicate rule ids accepted");
	auto many = QJsonArray();
	for (auto i = 0; i != 33; ++i) {
		many.push_back(NewRule(u"example.com"_q, u"mirror.example"_q, {}));
	}
	Require(!accepts(withRules(many)), "more than 32 rules accepted");
	Require(Validate(QByteArray()), "empty configuration rejected");
	std::cout << "PASS: Serein link rules" << std::endl;
}
