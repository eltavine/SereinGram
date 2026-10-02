#include "serein/snapshot/rules.h"
#include "serein/tests/require.h"

#include <doctest/doctest.h>

TEST_CASE("SnapshotConfig") {
	using namespace Serein::Snapshot;
	const auto stored = ReadStored(R"({"version":1,"background":true,)"
		R"("date":false,"headers":true,"reactions":true,"builtinTheme":true})");
	Require(stored && stored->background && !stored->date && stored->builtinTheme
		&& !stored->simpleReplies, "version 1 snapshot options not migrated");
	Require(ParseSnapshotConfig(SerializeSnapshotConfig(*stored)) == stored,
		"snapshot options do not round trip");
	Require(ReadStored(QByteArray()) == Defaults(),
		"empty snapshot options are not the defaults");
	const auto current = ReadStored(R"({"version":2,"background":false,)"
		R"("date":true,"headers":true,"reactions":false,"builtinTheme":false,)"
		R"("simpleReplies":true})");
	Require(current && current->simpleReplies && !current->background,
		"version 2 snapshot options not parsed");
	for (const auto bad : {
		R"({"version":3,"background":true,"date":true,"headers":true,"reactions":true,"builtinTheme":false,"simpleReplies":false})",
		R"({"version":2,"background":true,"date":true,"headers":true,"reactions":true,"builtinTheme":false})",
		R"({"version":1,"background":true,"date":true,"headers":true,"reactions":true,"builtinTheme":false,"simpleReplies":true})",
		R"({"version":1,"background":true,"date":true,"headers":true,"reactions":true})",
		R"({"version":1,"background":1,"date":true,"headers":true,"reactions":true,"builtinTheme":false})",
		R"({"version":1,"background":true,"date":true,"headers":true,"reactions":true,"builtinTheme":false,"x":true})",
	}) {
		Require(!ReadStored(bad), "malformed snapshot options accepted");
	}
}
