#pragma once

#include "serein/features/stories/model/post.h"

#include <gsl/pointers>
#include <rpl/variable.h>

#include <vector>

class UserData;

namespace Ui {
class VerticalLayout;
} // namespace Ui

namespace Window {
class SessionController;
} // namespace Window

namespace Serein::Stories {

struct AudienceValue {
	Audience audience = Audience::Everyone;
	std::vector<gsl::not_null<UserData*>> excluded;
	std::vector<gsl::not_null<UserData*>> selected;
};

[[nodiscard]] std::vector<AudienceRule> AudienceRulesFor(
	const AudienceValue &value);

void AddAudienceSection(
	gsl::not_null<Ui::VerticalLayout*> container,
	gsl::not_null<Window::SessionController*> controller,
	gsl::not_null<rpl::variable<AudienceValue>*> value);

} // namespace Serein::Stories
