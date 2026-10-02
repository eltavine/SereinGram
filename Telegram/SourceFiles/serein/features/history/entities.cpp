#include "serein/features/history/entities.h"

#include <array>
#include <utility>

namespace Serein::HistoryFeature {
namespace {

constexpr auto kNames = std::array{
	std::pair(EntityType::Url, "url"),
	std::pair(EntityType::CustomUrl, "custom_url"),
	std::pair(EntityType::Email, "email"),
	std::pair(EntityType::Hashtag, "hashtag"),
	std::pair(EntityType::Cashtag, "cashtag"),
	std::pair(EntityType::Mention, "mention"),
	std::pair(EntityType::MentionName, "mention_name"),
	std::pair(EntityType::CustomEmoji, "custom_emoji"),
	std::pair(EntityType::BotCommand, "bot_command"),
	std::pair(EntityType::MediaTimestamp, "media_timestamp"),
	std::pair(EntityType::Phone, "phone"),
	std::pair(EntityType::BankCard, "bank_card"),
	std::pair(EntityType::Bold, "bold"),
	std::pair(EntityType::Semibold, "semibold"),
	std::pair(EntityType::Italic, "italic"),
	std::pair(EntityType::Underline, "underline"),
	std::pair(EntityType::StrikeOut, "strike"),
	std::pair(EntityType::Code, "code"),
	std::pair(EntityType::Pre, "pre"),
	std::pair(EntityType::Blockquote, "blockquote"),
	std::pair(EntityType::Spoiler, "spoiler"),
	std::pair(EntityType::Subscript, "subscript"),
	std::pair(EntityType::Superscript, "superscript"),
	std::pair(EntityType::FormattedDate, "formatted_date"),
};

} // namespace

QString EntityName(EntityType type) {
	for (const auto &[known, name] : kNames) {
		if (known == type) {
			return QString::fromLatin1(name);
		}
	}
	return QString();
}

std::optional<EntityType> EntityTypeFromName(const QString &name) {
	for (const auto &[type, known] : kNames) {
		if (name == QLatin1StringView(known)) {
			return type;
		}
	}
	return std::nullopt;
}

} // namespace Serein::HistoryFeature
