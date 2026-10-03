#pragma once

#include "base/basic_types.h"
#include "serein/schema/gen/config/services.h"

namespace Serein::ServicesSchema {

inline constexpr auto kMaxSendTranslations = 500;

[[nodiscard]] SendTranslations ReadSendTranslations(const QByteArray &raw);
[[nodiscard]] QString SendLanguage(
	const SendTranslations &config,
	uint64 peer);
[[nodiscard]] std::optional<SendTranslations> WithSendLanguage(
	SendTranslations config,
	uint64 peer,
	const QString &language);

} // namespace Serein::ServicesSchema
