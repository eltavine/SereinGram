#include "serein/hooks/media/voice_denoise.h"
#include "serein/media/voice_denoise.h"

#include "core/application.h"
#include "serein/core/options.h"
#include "serein/schema/gen/settings/media.h"

#ifdef SEREIN_HAVE_RNNOISE
#include <rnnoise.h>
#endif // SEREIN_HAVE_RNNOISE

#include <algorithm>
#include <atomic>
#include <cmath>
#include <vector>

namespace Serein::Media {
namespace {

std::atomic<bool> Enabled = false;

class Watcher final : public QObject {
public:
	explicit Watcher(QObject *parent) : QObject(parent) {
		ForDevice().Value(
			kDenoiseVoiceMessages
		) | rpl::on_next([](bool enabled) {
			Enabled.store(enabled);
		}, _lifetime);
	}

private:
	rpl::lifetime _lifetime;

};

} // namespace

#ifdef SEREIN_HAVE_RNNOISE

class VoiceDenoiser final {
public:
	VoiceDenoiser()
	: _state(rnnoise_create(nullptr))
	, _frame(rnnoise_get_frame_size())
	, _input(std::max(_frame, 0))
	, _output(std::max(_frame, 0)) {
	}

	~VoiceDenoiser() {
		if (_state) {
			rnnoise_destroy(_state);
		}
	}

	void process(short *samples, int count) {
		if (!_state || _frame <= 0) {
			return;
		}
		for (auto offset = 0; offset + _frame <= count; offset += _frame) {
			for (auto i = 0; i != _frame; ++i) {
				_input[i] = float(samples[offset + i]);
			}
			rnnoise_process_frame(_state, _output.data(), _input.data());
			for (auto i = 0; i != _frame; ++i) {
				samples[offset + i] = short(std::clamp(
					std::lround(_output[i]),
					-32768L,
					32767L));
			}
		}
	}

private:
	DenoiseState *_state = nullptr;
	int _frame = 0;
	std::vector<float> _input;
	std::vector<float> _output;

};

void VoiceDenoiserDeleter::operator()(VoiceDenoiser *value) const {
	delete value;
}

VoiceDenoiserPointer CreateVoiceDenoiser() {
	return Enabled.load()
		? VoiceDenoiserPointer(new VoiceDenoiser())
		: VoiceDenoiserPointer();
}

void Denoise(VoiceDenoiser *denoiser, short *samples, int count) {
	if (denoiser) {
		denoiser->process(samples, count);
	}
}

#else

class VoiceDenoiser final {
};

void VoiceDenoiserDeleter::operator()(VoiceDenoiser *value) const {
	delete value;
}

VoiceDenoiserPointer CreateVoiceDenoiser() {
	return VoiceDenoiserPointer();
}

void Denoise(VoiceDenoiser*, short*, int) {
}

#endif // SEREIN_HAVE_RNNOISE

void StartVoiceDenoise() {
	new Watcher(&Core::App());
}

} // namespace Serein::Media
