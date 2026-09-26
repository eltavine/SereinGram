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
inline constexpr auto kExactMessageCounters = Option<bool>{
	"nagram.exactMessageCounters", Scope::Device, false,
	Category::Messages, "lng_nagram_exact_message_counters", kRefreshMessageView };
inline constexpr auto kHideMessageViews = Option<bool>{
	"nagram.hideMessageViews", Scope::Device, false,
	Category::Messages, "lng_nagram_hide_message_views", kRefreshMessageView };
inline constexpr auto kHideChannelSignature = Option<bool>{
	"nagram.hideChannelSignature", Scope::Device, false,
	Category::Messages, "lng_nagram_hide_channel_signature", kRefreshMessageView };
inline constexpr auto kHideEditedBadge = Option<bool>{
	"nagram.hideEditedBadge", Scope::Device, false,
	Category::Messages, "lng_nagram_hide_edited_badge", kRefreshMessageView };
inline const auto kEditedMark = Option<QString>{
	"nagram.editedMark", Scope::Device, QString(),
	Category::Messages, "lng_nagram_edited_mark", kRefreshMessageView };

inline void RegisterOptions(Registry &registry) {
	Expects(registry.Add(kSecondsInMessages));
	Expects(registry.Add(kShowForwardedMessageDate));
	Expects(registry.Add(kShowServiceTime));
	Expects(registry.Add(kShowMessageId));
	Expects(registry.Add(kExactMessageCounters));
	Expects(registry.Add(kHideMessageViews));
	Expects(registry.Add(kHideChannelSignature));
	Expects(registry.Add(kHideEditedBadge));
	Expects(registry.Add(kEditedMark));
}

} // namespace Nagram::Messages
