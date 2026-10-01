#pragma once

#include <memory>

namespace Serein::Media {

class VoiceDenoiser;

struct VoiceDenoiserDeleter {
	void operator()(VoiceDenoiser *value) const;
};

using VoiceDenoiserPointer = std::unique_ptr<
	VoiceDenoiser,
	VoiceDenoiserDeleter>;

[[nodiscard]] VoiceDenoiserPointer CreateVoiceDenoiser();
void Denoise(VoiceDenoiser *denoiser, short *samples, int count);

} // namespace Serein::Media
