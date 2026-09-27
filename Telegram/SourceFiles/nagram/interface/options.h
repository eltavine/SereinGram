#pragma once

#include "nagram/core/options.h"

namespace Nagram::Interface {

inline constexpr auto kRestart = static_cast<unsigned>(Flag::RequiresRestart);
inline constexpr auto kBubbleRoundness = Option<int>{
	"nagram.bubbleRoundness", Scope::Device, 0,
	Category::Interface, "lng_nagram_bubble_roundness", kRestart,
	[](const int &value) { return !value || (value >= 10 && value <= 100); } };
inline constexpr auto kAvatarRoundness = Option<int>{
	"nagram.avatarRoundness", Scope::Device, 0,
	Category::Interface, "lng_nagram_avatar_roundness", kRestart,
	[](const int &value) { return !value || (value >= 10 && value <= 100); } };
inline constexpr auto kUniformAvatarShapes = Option<bool>{
	"nagram.uniformAvatarShapes", Scope::Device, false,
	Category::Interface, "lng_nagram_uniform_avatar_shapes", kRestart };

inline void RegisterOptions(Registry &registry) {
	Expects(registry.Add(kBubbleRoundness));
	Expects(registry.Add(kAvatarRoundness));
	Expects(registry.Add(kUniformAvatarShapes));
}

} // namespace Nagram::Interface
