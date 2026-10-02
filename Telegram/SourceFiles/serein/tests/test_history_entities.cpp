#include "serein/features/history/entities.h"
#include "base/basic_types.h"
#include "serein/tests/require.h"

#include <doctest/doctest.h>
#include <utility>
#include <vector>

TEST_CASE("HistoryEntities") {
	using Serein::HistoryFeature::EntityName;
	using Serein::HistoryFeature::EntityTypeFromName;

	// The local history database stores these names; renaming one loses
	// the formatting of every record saved before the change.
	const auto stored = std::vector<std::pair<EntityType, const char*>>{
		{ EntityType::Url, "url" },
		{ EntityType::CustomUrl, "custom_url" },
		{ EntityType::Email, "email" },
		{ EntityType::Hashtag, "hashtag" },
		{ EntityType::Cashtag, "cashtag" },
		{ EntityType::Mention, "mention" },
		{ EntityType::MentionName, "mention_name" },
		{ EntityType::CustomEmoji, "custom_emoji" },
		{ EntityType::BotCommand, "bot_command" },
		{ EntityType::MediaTimestamp, "media_timestamp" },
		{ EntityType::Phone, "phone" },
		{ EntityType::BankCard, "bank_card" },
		{ EntityType::Bold, "bold" },
		{ EntityType::Semibold, "semibold" },
		{ EntityType::Italic, "italic" },
		{ EntityType::Underline, "underline" },
		{ EntityType::StrikeOut, "strike" },
		{ EntityType::Code, "code" },
		{ EntityType::Pre, "pre" },
		{ EntityType::Blockquote, "blockquote" },
		{ EntityType::Spoiler, "spoiler" },
		{ EntityType::Subscript, "subscript" },
		{ EntityType::Superscript, "superscript" },
		{ EntityType::FormattedDate, "formatted_date" },
	};
	for (const auto &[type, name] : stored) {
		Require(EntityName(type) == QString::fromLatin1(name),
			"stored entity name changed");
		Require(EntityTypeFromName(QString::fromLatin1(name)) == type,
			"stored entity name not read back");
	}
	for (const auto type : {
			EntityType::Invalid,
			EntityType::Colorized,
			EntityType::Marked }) {
		Require(EntityName(type).isEmpty(), "display-only entity stored");
	}
	Require(!EntityTypeFromName(u"URL"_q), "entity names ignore case");
	Require(!EntityTypeFromName(u"colorized"_q), "unknown entity read");
	Require(!EntityTypeFromName(QString()), "empty entity name read");
}
