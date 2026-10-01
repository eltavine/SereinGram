#pragma once

namespace Ui {
class VerticalLayout;
} // namespace Ui

namespace Serein {

void GuardSettings(not_null<Ui::VerticalLayout*> content, Fn<void()> build);
void StartSettingsLock();

} // namespace Serein
