#pragma once

#include <gsl/pointers>

#include <optional>

class History;
struct MsgId;

namespace Main {
class Session;
} // namespace Main

namespace Ui {
class SettingsButton;
} // namespace Ui

namespace Serein::Hooks {

[[nodiscard]] bool AllowOnline(gsl::not_null<Main::Session*> session);
[[nodiscard]] bool AllowReadReceipt(gsl::not_null<Main::Session*> session);
[[nodiscard]] bool AllowTyping(gsl::not_null<Main::Session*> session);
[[nodiscard]] bool AllowStoryView(gsl::not_null<Main::Session*> session);
[[nodiscard]] bool AllowViewIncrement(gsl::not_null<Main::Session*> session);


[[nodiscard]] bool ReadInboxLocally(
	gsl::not_null<History*> history,
	MsgId tillId,
	std::optional<int> stillUnread);

void BindGhostToggle(
	gsl::not_null<Ui::SettingsButton*> button,
	gsl::not_null<Main::Session*> session);

} // namespace Serein::Hooks
