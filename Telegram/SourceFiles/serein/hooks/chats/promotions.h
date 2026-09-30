#pragma once

namespace Dialogs::TopBarSuggestions {
enum class Priority : int;
} // namespace Dialogs::TopBarSuggestions

namespace Serein::Chats {

[[nodiscard]] bool HideSponsoredMessages();
[[nodiscard]] bool HideProxySponsor();
[[nodiscard]] bool HideSuggestion(Dialogs::TopBarSuggestions::Priority priority);

} // namespace Serein::Chats
