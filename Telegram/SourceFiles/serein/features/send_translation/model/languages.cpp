#include "serein/features/send_translation/model/languages.h"

#include "serein/schema/gen/settings/services.h"

namespace Serein::ServicesSchema {
namespace {

[[nodiscard]] QString Key(uint64 peer) {
	return QString::number(peer);
}

} // namespace

SendTranslations ReadSendTranslations(const QByteArray &raw) {
	return raw.isEmpty()
		? SendTranslations()
		: ParseSendTranslations(raw).value_or(SendTranslations());
}

QString SendLanguage(const SendTranslations &config, uint64 peer) {
	const auto i = config.languages.find(Key(peer));
	return (i != end(config.languages)) ? i->second : QString();
}

std::optional<SendTranslations> WithSendLanguage(
		SendTranslations config,
		uint64 peer,
		const QString &language) {
	if (!peer) {
		return std::nullopt;
	} else if (language.isEmpty()) {
		config.languages.erase(Key(peer));
	} else if (config.languages.contains(Key(peer))
		|| config.languages.size() < kMaxSendTranslations) {
		config.languages[Key(peer)] = language;
	} else {
		return std::nullopt;
	}
	return ParseSendTranslations(SerializeSendTranslations(config))
		? std::make_optional(std::move(config))
		: std::nullopt;
}

} // namespace Serein::ServicesSchema

namespace Serein::ServiceSettings {

bool ValidSendTranslationsBytes(const QByteArray &value) {
	return value.isEmpty()
		|| ServicesSchema::ParseSendTranslations(value).has_value();
}

} // namespace Serein::ServiceSettings
