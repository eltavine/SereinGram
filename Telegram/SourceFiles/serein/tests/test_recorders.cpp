#include "serein/privacy/recorders.h"
#include "base/basic_types.h"
#include "serein/tests/require.h"

#include <doctest/doctest.h>

TEST_CASE("Recorders") {
	using namespace Serein::Privacy;
	for (const auto name : {
			"obs",
			"OBS",
			"obs64.exe",
			"XSplit.Core.exe",
			"Streamlabs OBS.exe",
			"vMix64.exe",
			" bdcam.exe ",
			"simplescreenrec",
			"gpu-screen-reco" }) {
		Require(IsRecorderProcess(QString::fromLatin1(name)),
			"recording app not recognized");
	}
	for (const auto name : {
			"",
			".exe",
			"telegram",
			"obsidian",
			"notobs",
			"vmix-helper",
			"simplescreen" }) {
		Require(!IsRecorderProcess(QString::fromLatin1(name)),
			"unrelated process taken for a recording app");
	}
	Require(RecorderRunning({ u"Finder"_q, u"OBS"_q }), "running recorder missed");
	Require(!RecorderRunning({}) && !RecorderRunning({ u"Finder"_q }),
		"recorder reported without one running");
}
