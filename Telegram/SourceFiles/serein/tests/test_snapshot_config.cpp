#include "serein/schema/gen/config/snapshot.h"

#include <stdexcept>

namespace {

void Require(bool value, const char *message) {
	if (!value) {
		throw std::runtime_error(message);
	}
}

} // namespace

void TestSnapshotConfig() {
	using namespace Serein::Snapshot;
	const auto stored = ParseSnapshotConfig(R"({"version":1,"background":true,)"
		R"("date":false,"headers":true,"reactions":true,"builtinTheme":true})");
	Require(stored && stored->background && !stored->date && stored->builtinTheme,
		"stored snapshot options not parsed");
	Require(ParseSnapshotConfig(SerializeSnapshotConfig(*stored)) == stored,
		"snapshot options do not round trip");
	for (const auto bad : {
		R"({"version":2,"background":true,"date":true,"headers":true,"reactions":true,"builtinTheme":false})",
		R"({"version":1,"background":true,"date":true,"headers":true,"reactions":true})",
		R"({"version":1,"background":1,"date":true,"headers":true,"reactions":true,"builtinTheme":false})",
		R"({"version":1,"background":true,"date":true,"headers":true,"reactions":true,"builtinTheme":false,"x":true})",
	}) {
		Require(!ParseSnapshotConfig(bad), "malformed snapshot options accepted");
	}
}
