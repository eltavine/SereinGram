#pragma once

#include <rpl/producer.h>

#include <gsl/pointers>

namespace Ui {
class RpWidget;
} // namespace Ui

namespace Serein::Messages {

[[nodiscard]] bool ShowPremiumBadges();
template <typename Content>
[[nodiscard]] rpl::producer<Content> VisibleBadgeContent(
	rpl::producer<Content> content);
void AttachPremiumRefresh(gsl::not_null<Ui::RpWidget*> widget);

} // namespace Serein::Messages
