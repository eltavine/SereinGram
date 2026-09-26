#pragma once

#include "nagram/core/options.h"

namespace Nagram::Messages {

inline constexpr auto kRefreshMessageView = static_cast<unsigned>(
	Flag::RefreshMessageView);

inline constexpr auto kSecondsInMessages = Option<bool>{
	"nagram.secondsInMessages", Scope::Device, false,
	Category::Messages, "lng_nagram_seconds_in_messages", kRefreshMessageView };
inline constexpr auto kShowForwardedMessageDate = Option<bool>{
	"nagram.showForwardedMessageDate", Scope::Device, false,
	Category::Messages, "lng_nagram_show_forwarded_message_date", kRefreshMessageView };
inline constexpr auto kShowServiceTime = Option<bool>{
	"nagram.showServiceTime", Scope::Device, false,
	Category::Messages, "lng_nagram_show_service_time", kRefreshMessageView };
inline constexpr auto kShowMessageId = Option<bool>{
	"nagram.showMessageId", Scope::Device, false,
	Category::Messages, "lng_nagram_show_message_id", kRefreshMessageView };

inline void RegisterOptions(Registry &registry) {
	Expects(registry.Add(kSecondsInMessages));
	Expects(registry.Add(kShowForwardedMessageDate));
	Expects(registry.Add(kShowServiceTime));
	Expects(registry.Add(kShowMessageId));
}

} // namespace Nagram::Messages
