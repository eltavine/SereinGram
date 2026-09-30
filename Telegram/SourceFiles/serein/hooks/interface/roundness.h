#pragma once

#include "base/basic_types.h"

#include <QtGui/QImage>

#include <array>
#include <optional>

class PeerData;
namespace Media::Streaming { class Instance; }
namespace Ui { enum class PeerUserpicShape : uint8; }

namespace Serein::Interface {

void StartRoundness();
[[nodiscard]] int BubblePercent();
[[nodiscard]] int AdjustBubbleRadius(int radius);
[[nodiscard]] std::optional<int> AvatarRadius(
	int size,
	Ui::PeerUserpicShape shape);
[[nodiscard]] Ui::PeerUserpicShape ResolvedAvatarShape(
	Ui::PeerUserpicShape shape,
	PeerData *peer);
[[nodiscard]] QImage RoundedAvatarFrame(
	Media::Streaming::Instance &stream,
	int size,
	int radius,
	std::array<QImage, 4> &corners);

} // namespace Serein::Interface
