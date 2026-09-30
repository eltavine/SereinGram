#pragma once

#include "ui/text/text_entity.h"
#include "lang/translate_provider.h"
#include "serein/hooks/services/translation.h"

namespace Main {
class Session;
} // namespace Main

namespace Serein {

struct ServiceDefinition;

struct TranslationPart {
	int start = 0;
	int length = 0;
	int index = -1;
};

struct TranslationPlan {
	TextWithEntities original;
	std::vector<TranslationPart> parts;
	QStringList texts;
};

[[nodiscard]] std::optional<TranslationPlan> PlanTranslation(TextWithEntities text);
[[nodiscard]] std::optional<TextWithEntities> ApplyTranslation(
	const TranslationPlan &plan,
	const QStringList &translated);
[[nodiscard]] std::unique_ptr<Ui::TranslateProvider> CreateServiceTranslateProvider(
	const ServiceDefinition &service,
	Fn<void(QString)> error);

} // namespace Serein
