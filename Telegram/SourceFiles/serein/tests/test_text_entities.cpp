#include "serein/display/text_entities.h"
#include "serein/tests/require.h"

#include <doctest/doctest.h>

TEST_CASE("VerbatimEntities") {
	using Serein::Display::VerbatimEntity;
	for (const auto type : {
			EntityType::Url,
			EntityType::CustomUrl,
			EntityType::Email,
			EntityType::Hashtag,
			EntityType::Cashtag,
			EntityType::Mention,
			EntityType::MentionName,
			EntityType::CustomEmoji,
			EntityType::BotCommand,
			EntityType::MediaTimestamp,
			EntityType::Phone,
			EntityType::BankCard,
			EntityType::Code,
			EntityType::Pre,
			EntityType::FormattedDate }) {
		Require(VerbatimEntity(type), "text inside this entity is rewritten");
	}
	for (const auto type : {
			EntityType::Invalid,
			EntityType::Colorized,
			EntityType::Bold,
			EntityType::Semibold,
			EntityType::Italic,
			EntityType::Underline,
			EntityType::StrikeOut,
			EntityType::Blockquote,
			EntityType::Spoiler,
			EntityType::Subscript,
			EntityType::Superscript,
			EntityType::Marked }) {
		Require(!VerbatimEntity(type), "formatting blocks text changes");
	}
}
