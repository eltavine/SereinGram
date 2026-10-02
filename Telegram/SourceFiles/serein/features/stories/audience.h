#pragma once

#include "serein/features/stories/model/post.h"

#include <gsl/pointers>
#include <rpl/variable.h>

#include <memory>
#include <vector>

class UserData;

namespace ChatHelpers {
class Show;
} // namespace ChatHelpers

namespace Main {
class Session;
} // namespace Main

namespace Ui {
class VerticalLayout;
} // namespace Ui

namespace Serein::Stories {

struct AudienceValue {
	Audience audience = Audience::Everyone;
	std::vector<gsl::not_null<UserData*>> excluded;
	std::vector<gsl::not_null<UserData*>> selected;
};

[[nodiscard]] std::vector<AudienceRule> AudienceRulesFor(
	const AudienceValue &value);
[[nodiscard]] AudienceValue AudienceValueFor(
	gsl::not_null<Main::Session*> session,
	const ParsedAudience &parsed);

void AddAudienceSection(
	gsl::not_null<Ui::VerticalLayout*> container,
	std::shared_ptr<ChatHelpers::Show> show,
	gsl::not_null<rpl::variable<AudienceValue>*> value);

} // namespace Serein::Stories
