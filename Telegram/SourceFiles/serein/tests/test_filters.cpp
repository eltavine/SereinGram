#include "serein/filters/model.h"

#include <QtCore/QElapsedTimer>
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

QJsonObject Rule(const QString &pattern, const QString &action) {
	return {
		{ u"id"_q, u"00000000-0000-0000-0000-000000000001"_q },
		{ u"title"_q, u"test"_q },
		{ u"pattern"_q, pattern },
		{ u"replacement"_q, u"bar"_q },
		{ u"action"_q, action },
		{ u"enabled"_q, true },
		{ u"caseInsensitive"_q, false },
		{ u"reversed"_q, false },
	};
}

QJsonObject LegacyDefaults() {
	return {
		{ u"version"_q, 1 },
		{ u"enabled"_q, false },
		{ u"filterOutgoing"_q, false },
		{ u"hideBlocked"_q, false },
		{ u"stripZalgo"_q, false },
		{ u"hiddenAuthors"_q, QJsonArray() },
		{ u"excludedPeers"_q, QJsonArray() },
		{ u"rules"_q, QJsonArray() },
	};
}

QByteArray Config(const QJsonObject &rule) {
	auto config = LegacyDefaults();
	config.insert(u"enabled"_q, true);
	config.insert(u"rules"_q, QJsonArray{ rule });
	return QJsonDocument(config).toJson(QJsonDocument::Compact);
}

} // namespace

void TestFilterScopes() {
	using namespace Serein::Filters;
	auto scoped = Rule(u"foo"_q, u"replace"_q);
	scoped.insert(u"peers"_q, QJsonArray{ u"42"_q });
	auto config = QJsonDocument::fromJson(Config(scoped)).object();
	config.insert(u"version"_q, 2);
	const auto raw = QJsonDocument(config).toJson(QJsonDocument::Compact);
	Require(Validate(raw), "chat scoped rule rejected");
	Require(Apply(raw, { u"foo"_q }, {}, u"42"_q, false, false).text.text
			== u"bar"_q,
		"chat scoped rule skipped in its chat");
	Require(Apply(raw, { u"foo"_q }, {}, u"7"_q, false, false).text.text
			== u"foo"_q,
		"chat scoped rule applied in another chat");
	Require(Apply(raw, { u"foo"_q }, {}, u"42"_q, false, false, {}, {},
			u"42:7"_q).text.text == u"bar"_q,
		"chat scoped rule skipped inside a topic of its chat");
	auto topicRule = Rule(u"foo"_q, u"replace"_q);
	topicRule.insert(u"peers"_q, QJsonArray{ u"42:7"_q });
	auto topicConfig = QJsonDocument::fromJson(Config(topicRule)).object();
	topicConfig.insert(u"version"_q, 2);
	const auto topicRaw = QJsonDocument(topicConfig).toJson(
		QJsonDocument::Compact);
	Require(Validate(topicRaw), "topic scoped rule rejected");
	const auto inTopic = [&](const QString &topic) {
		return Apply(topicRaw, { u"foo"_q }, {}, u"42"_q, false, false, {},
			{}, topic).text.text;
	};
	Require(inTopic(u"42:7"_q) == u"bar"_q
		&& inTopic(u"42:8"_q) == u"foo"_q
		&& inTopic(QString()) == u"foo"_q,
		"topic scoped rule applied outside its topic");
	topicRule.insert(u"peers"_q, QJsonArray{ u"42:0"_q });
	auto badTopic = QJsonDocument::fromJson(Config(topicRule)).object();
	badTopic.insert(u"version"_q, 2);
	Require(!Validate(QJsonDocument(badTopic).toJson(QJsonDocument::Compact)),
		"malformed topic scope accepted");
	const auto upgraded = ReadRules(Config(Rule(u"foo"_q, u"mask"_q)));
	Require(upgraded && upgraded->rules.size() == 1
		&& upgraded->rules[0].peers.empty(),
		"version 1 filter rules not upgraded");
	auto missing = QJsonDocument::fromJson(Config(Rule(u"foo"_q, u"mask"_q)))
		.object();
	missing.insert(u"version"_q, 2);
	Require(!Validate(QJsonDocument(missing).toJson(QJsonDocument::Compact)),
		"version 2 rule without peers accepted");
	scoped.insert(u"peers"_q, QJsonArray{ u"0"_q });
	auto badPeer = QJsonDocument::fromJson(Config(scoped)).object();
	badPeer.insert(u"version"_q, 2);
	Require(!Validate(QJsonDocument(badPeer).toJson(QJsonDocument::Compact)),
		"malformed chat scope accepted");

	const auto list = ReadRuleList(QJsonDocument(QJsonObject{
		{ u"version"_q, 1 },
		{ u"rules"_q, QJsonArray{ Rule(u"foo"_q, u"hide"_q) } },
	}).toJson(QJsonDocument::Compact));
	Require(list && list->size() == 1, "version 1 rule list not imported");
	Require(ReadRuleList(WriteRuleList(*list)) == list,
		"rule list does not round trip");
	Require(!ReadRuleList(QJsonDocument(QJsonObject{
		{ u"version"_q, 1 },
		{ u"rules"_q, QJsonArray{ Rule(u"("_q, u"hide"_q) } },
	}).toJson(QJsonDocument::Compact)), "rule list with a broken regex accepted");
	Require(!ReadRuleList("{\"version\":1}"), "rule list without rules accepted");
}

void TestSharedRules() {
	using namespace Serein::Filters;
	const auto shared = std::vector<FilterRule>{ FilterRule{
		.id = u"00000000-0000-0000-0000-000000000002"_q,
		.title = u"shared"_q,
		.pattern = u"spam"_q,
		.replacement = u"ham"_q,
		.enabled = true,
		.action = u"replace"_q,
	} };
	Require(ValidRuleList(WriteRuleList(shared))
		&& ValidRuleList({})
		&& !ValidRuleList("{"), "wrong rule list validation");
	auto enabled = LegacyDefaults();
	enabled.insert(u"enabled"_q, true);
	const auto on = QJsonDocument(enabled).toJson(QJsonDocument::Compact);
	const auto off = QJsonDocument(LegacyDefaults()).toJson(
		QJsonDocument::Compact);
	const auto text = TextWithEntities{ u"spam"_q };
	const auto apply = [&](const QByteArray &raw) {
		return Apply(raw, text, {}, u"7"_q, false, false, {}, shared)
			.text.text;
	};
	Require(apply(on) == u"ham"_q, "shared rule skipped");
	Require(apply(off) == u"spam"_q, "shared rule ran with filtering off");
	Require(apply(Config(Rule(u"spam"_q, u"replace"_q))) == u"bar"_q,
		"shared rules ran before local ones");
}

void TestFilters() {
	using namespace Serein::Filters;
	TestFilterScopes();
	TestSharedRules();
	Require(Validate({}), "empty filter config rejected");
	const auto replace = Config(Rule(u"foo"_q, u"replace"_q));
	Require(Validate(replace), "valid filter config rejected");
	const auto replaced = Apply(replace, { u"foo foo"_q }, {}, {}, false, false);
	Require(replaced.text.text == u"bar bar"_q && replaced.matches == 2,
		"replacement or match count incorrect");
	const auto hidden = Apply(Config(Rule(u"foo"_q, u"hide"_q)),
		{ u"foo"_q }, {}, {}, false, false);
	Require(hidden.hidden, "hide action did not hide locally");
	Require(!Validate(Config(Rule(u"("_q, u"hide"_q))),
		"invalid regex accepted");
	const auto excessive = Apply(Config(Rule(u"a"_q, u"replace"_q)),
		{ QString(300, u'a') }, {}, {}, false, false);
	Require(!excessive.error.isEmpty() && excessive.text.text == QString(300, u'a'),
		"match budget did not preserve original text");
	auto clock = QElapsedTimer();
	clock.start();
	const auto adversarial = Apply(
		Config(Rule(u"(a+)+$"_q, u"hide"_q)),
		{ QString(16000, u'a') + u"X"_q }, {}, {}, false, false);
	Require(clock.elapsed() < 500,
		"adversarial regex exceeded bounded test duration");
	Require(!adversarial.hidden && !adversarial.error.isEmpty(),
		"regex work limit did not report an actionable failure");
	const auto rule = Rule(u"foo"_q, u"mask"_q);
	const auto withRule = [&](const QJsonObject &changed) {
		return Validate(Config(changed));
	};
	const auto withField = [&](const QString &key, const QJsonValue &value) {
		auto changed = rule;
		changed.insert(key, value);
		return withRule(changed);
	};
	const auto withConfig = [&](const QString &key, const QJsonValue &value) {
		auto config = QJsonDocument::fromJson(Config(rule)).object();
		if (value.isUndefined()) {
			config.remove(key);
		} else {
			config.insert(key, value);
		}
		return Validate(QJsonDocument(config).toJson(QJsonDocument::Compact));
	};
	Require(!withConfig(u"stripZalgo"_q, QJsonValue::Undefined),
		"filter config without a key accepted");
	Require(!withConfig(u"extra"_q, 1), "filter config with an unknown key accepted");
	Require(!withConfig(u"version"_q, 2), "unknown filter version accepted");
	Require(!withField(u"action"_q, u"drop"_q), "unknown filter action accepted");
	Require(!withField(u"reversed"_q, true), "reversed mask rule accepted");
	auto reversedHide = Rule(u"foo"_q, u"hide"_q);
	reversedHide.insert(u"reversed"_q, true);
	Require(withRule(reversedHide), "reversed hide rule rejected");
	Require(!withField(u"pattern"_q, u"("_q), "invalid regex accepted");
	Require(!withField(u"id"_q, u"00000000-0000-0000-0000-00000000000A"_q),
		"uppercase rule id accepted");
	Require(!withField(u"id"_q, u"00000000-0000-0000-0000-000000000000"_q),
		"nil rule id accepted");
	Require(!withField(u"title"_q, QString()), "empty rule title accepted");
	Require(!withField(u"title"_q, QString(129, u'x')), "long rule title accepted");
	Require(withField(u"replacement"_q, QString()), "empty replacement rejected");
	auto partial = rule;
	partial.remove(u"enabled"_q);
	Require(!withRule(partial), "rule without enabled accepted");
	Require(withConfig(u"hiddenAuthors"_q, QJsonArray{ u"123"_q }),
		"valid hidden author rejected");
	for (const auto &bad : { u"0"_q, u"01"_q, u"-5"_q, u"99999999999999999999"_q }) {
		Require(!withConfig(u"hiddenAuthors"_q, QJsonArray{ bad }),
			"invalid hidden author accepted");
	}
	Require(!withConfig(u"excludedPeers"_q, QJsonArray{ u"7"_q, u"7"_q }),
		"duplicate excluded peers accepted");
	Require(!withConfig(u"excludedPeers"_q, QJsonArray{ 7 }),
		"numeric excluded peer accepted");
	Require(!withConfig(u"rules"_q, QJsonArray{ rule, rule }),
		"duplicate rule ids accepted");
	std::cout << "PASS: Serein filter budgets and projection" << std::endl;
}
