#pragma once

#include <memory>

namespace Ui {
class Show;
} // namespace Ui

namespace Serein::Filters {

void UpdateRuleSubscription(std::shared_ptr<Ui::Show> show);
void StartRuleSubscription();

} // namespace Serein::Filters
