#include "serein/privacy/recorders.h"

#include "base/basic_types.h"

#include <algorithm>
#include <array>

namespace Serein::Privacy {
namespace {

// Linux reports at most this many characters of a process name.
constexpr auto kTruncatedName = 15;

constexpr auto kRecorders = std::array{
	"obs",
	"obs32",
	"obs64",
	"obs-studio",
	"streamlabs obs",
	"streamlabs desktop",
	"slobs",
	"xsplit.core",
	"bdcam",
	"vmix",
	"vmix64",
	"twitchstudio",
	"twitch studio",
	"prismlivestudio",
	"screenflow",
	"simplescreenrecorder",
	"kazam",
	"vokoscreen",
	"vokoscreenng",
	"kooha",
	"gpu-screen-recorder",
	"wf-recorder",
	"recordmydesktop",
};

} // namespace

bool IsRecorderProcess(const QString &name) {
	auto normalized = name.trimmed().toLower();
	if (normalized.endsWith(u".exe"_q)) {
		normalized.chop(4);
	}
	if (normalized.isEmpty()) {
		return false;
	}
	return std::any_of(kRecorders.begin(), kRecorders.end(), [&](
			const char *recorder) {
		const auto known = QString::fromLatin1(recorder);
		return (known == normalized)
			|| (normalized.size() == kTruncatedName
				&& known.startsWith(normalized));
	});
}

bool RecorderRunning(const std::vector<QString> &names) {
	return std::any_of(names.begin(), names.end(), IsRecorderProcess);
}

} // namespace Serein::Privacy
