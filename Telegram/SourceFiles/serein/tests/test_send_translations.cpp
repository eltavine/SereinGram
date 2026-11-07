#include "serein/features/send_translation/model/languages.h"
#include "serein/schema/gen/settings/services.h"

#include <doctest/doctest.h>

namespace Serein::ServicesSchema {
namespace {

constexpr auto kPeer = uint64(1234567);
constexpr auto kOther = uint64(7654321);

} // namespace

TEST_CASE("SendTranslations") {
	SUBCASE("empty and invalid data read as no languages") {
		CHECK(ReadSendTranslations(QByteArray()) == SendTranslations());
		CHECK(ReadSendTranslations("not json") == SendTranslations());
		CHECK(SendLanguage(SendTranslations(), kPeer).isEmpty());
	}
	SUBCASE("languages are set, replaced and removed per chat") {
		auto config = WithSendLanguage({}, kPeer, u"en_US"_q);
		REQUIRE(config);
		CHECK(SendLanguage(*config, kPeer) == u"en_US"_q);
		CHECK(SendLanguage(*config, kOther).isEmpty());
		config = WithSendLanguage(*config, kPeer, u"ja_JP"_q);
		REQUIRE(config);
		CHECK(SendLanguage(*config, kPeer) == u"ja_JP"_q);
		config = WithSendLanguage(*config, kPeer, QString());
		REQUIRE(config);
		CHECK(config->languages.empty());
	}
	SUBCASE("documents round trip and reject bad values") {
		const auto config = WithSendLanguage({}, kPeer, u"zh_CN"_q);
		REQUIRE(config);
		const auto raw = SerializeSendTranslations(*config);
		CHECK(ReadSendTranslations(raw) == *config);
		CHECK(ServiceSettings::ValidSendTranslationsBytes(raw));
		CHECK(ServiceSettings::ValidSendTranslationsBytes(QByteArray()));
		CHECK(!ServiceSettings::ValidSendTranslationsBytes("[]"));
		CHECK(!WithSendLanguage({}, kPeer, u"English"_q));
		CHECK(!WithSendLanguage({}, 0, u"en_US"_q));
	}
	SUBCASE("the number of chats is limited") {
		auto config = SendTranslations();
		for (auto i = 0; i != kMaxSendTranslations; ++i) {
			config.languages.emplace(QString::number(i + 1), u"en_US"_q);
		}
		CHECK(!WithSendLanguage(config, kMaxSendTranslations + 1, u"en_US"_q));
		CHECK(WithSendLanguage(config, 1, u"de_DE"_q));
		CHECK(WithSendLanguage(config, 1, QString()));
	}
}

} // namespace Serein::ServicesSchema
