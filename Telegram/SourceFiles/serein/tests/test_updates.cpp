#include "serein/features/updates/model/release.h"
#include "base/basic_types.h"
#include "serein/tests/require.h"

#include <doctest/doctest.h>
#include <iostream>

TEST_CASE("Updates") {
	using namespace Serein::Updates;
	const auto release = ParseLatestRelease(R"({
		"tag_name": "v7.2.11",
		"html_url": "https://github.com/eltavine/SereinGram/releases/tag/v7.2.11",
		"draft": false,
		"prerelease": false
	})");
	Require(release && release->tag == u"v7.2.11"_q
		&& release->url.startsWith(u"https://github.com/"_q),
		"latest release not parsed");
	for (const auto &bad : {
		R"({"tag_name":"v8","html_url":"https://github.com/a/b","prerelease":true})",
		R"({"tag_name":"v8","html_url":"https://github.com/a/b","draft":true})",
		R"({"tag_name":"v8","html_url":"http://github.com/a/b"})",
		R"({"tag_name":"v8","html_url":"https://evil.example/a/b"})",
		R"({"tag_name":"nightly","html_url":"https://github.com/a/b"})",
	}) {
		Require(!ParseLatestRelease(bad), "unsafe or unstable release accepted");
	}
	const auto nightly = ParseNightlyRelease(R"({
		"tag_name": "nightly",
		"target_commitish": "0123456789abcdef0123456789abcdef01234567",
		"html_url": "https://github.com/eltavine/SereinGram/releases/tag/nightly",
		"draft": false,
		"prerelease": true
	})");
	Require(nightly
		&& nightly->commit == u"0123456789abcdef0123456789abcdef01234567"_q
		&& nightly->url.startsWith(u"https://github.com/"_q),
		"nightly release not parsed");
	for (const auto &bad : {
		R"({"tag_name": "v8",
			"target_commitish": "0123456789abcdef0123456789abcdef01234567",
			"html_url": "https://github.com/a/b"})",
		R"({"tag_name": "nightly",
			"target_commitish": "develop",
			"html_url": "https://github.com/a/b"})",
		R"({"tag_name": "nightly",
			"target_commitish": "0123456789abcdef0123456789abcdef01234567",
			"html_url": "https://github.com/a/b",
			"draft": true})",
		R"({"tag_name": "nightly",
			"target_commitish": "0123456789abcdef0123456789abcdef01234567",
			"html_url": "https://evil.example/a/b"})",
	}) {
		Require(!ParseNightlyRelease(bad), "unexpected nightly release accepted");
	}
	Require(VersionParts(u"v7.2.10.1"_q) == std::vector<int>{ 7, 2, 10, 1 },
		"four part version not parsed");
	Require(VersionParts(u"7.2.10-beta"_q) == std::vector<int>{ 7, 2, 10 },
		"version suffix not ignored");
	Require(VersionParts(u"beta"_q).empty(), "version without numbers parsed");
	Require(IsNewer(u"v7.2.10.1"_q, u"7.2.10"_q), "extra part not newer");
	Require(IsNewer(u"7.3"_q, u"7.2.10"_q), "minor bump not newer");
	Require(!IsNewer(u"7.2.10"_q, u"7.2.10"_q), "same version newer");
	Require(!IsNewer(u"7.2.9"_q, u"7.2.10"_q), "older version newer");
	Require(!IsNewer(u"beta"_q, u"7.2.10"_q), "invalid version newer");
	std::cout << "PASS: Serein update checks" << std::endl;
}
