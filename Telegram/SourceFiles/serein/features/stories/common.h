#pragma once

#include "serein/features/stories/model/post.h"

#include <gsl/pointers>

#include <memory>
#include <vector>

namespace ChatHelpers {
class Show;
} // namespace ChatHelpers

namespace Main {
class Session;
} // namespace Main

namespace Ui {
class GenericBox;
class InputField;
} // namespace Ui

namespace Serein::Stories {

[[nodiscard]] QString ErrorText(const QString &type);
[[nodiscard]] TextWithEntities PrepareCaption(const TextWithTags &caption);
[[nodiscard]] MTPVector<MTPInputPrivacyRule> PrivacyRules(
	gsl::not_null<Main::Session*> session,
	const std::vector<AudienceRule> &rules);
[[nodiscard]] gsl::not_null<Ui::InputField*> AddCaptionField(
	gsl::not_null<Ui::GenericBox*> box,
	std::shared_ptr<ChatHelpers::Show> show);

} // namespace Serein::Stories
