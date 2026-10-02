#include "serein/schema/gen/config/sticker_catalog.h"
#include "serein/tests/require.h"

#include <doctest/doctest.h>

TEST_CASE("StickerCatalog") {
	using namespace Serein::MediaSchema;
	const auto parsed = ParseStickerCatalogFile(R"({
		"format": "serein-sticker-catalog",
		"version": 1,
		"sets": [
			{ "shortName": "Animals", "title": "Animals", "type": "stickers" },
			{ "shortName": "smiles_2", "title": "", "type": "emoji" }
		]
	})");
	Require(parsed && parsed->sets.size() == 2
		&& parsed->sets[1].type == QString::fromLatin1("emoji"),
		"sticker catalog not parsed");
	Require(ParseStickerCatalogFile(SerializeStickerCatalogFile(*parsed)) == parsed,
		"sticker catalog does not round trip");
	for (const auto bad : {
		R"({"format":"other","version":1,"sets":[]})",
		R"({"format":"serein-sticker-catalog","version":2,"sets":[]})",
		R"({"format":"serein-sticker-catalog","version":1})",
		R"({"format":"serein-sticker-catalog","version":1,"sets":[],"x":1})",
		R"({"format":"serein-sticker-catalog","version":1,"sets":[{"shortName":"a b","title":"","type":"stickers"}]})",
		R"({"format":"serein-sticker-catalog","version":1,"sets":[{"shortName":"a","title":"","type":"gifs"}]})",
		R"({"format":"serein-sticker-catalog","version":1,"sets":[{"shortName":"a","title":"line\nbreak","type":"stickers"}]})",
		R"({"format":"serein-sticker-catalog","version":1,"sets":[{"shortName":"Pack","title":"","type":"stickers"},{"shortName":"pack","title":"","type":"masks"}]})",
	}) {
		Require(!ParseStickerCatalogFile(bad), "malformed sticker catalog accepted");
	}
	auto many = StickerCatalogFile{ .format = QString::fromLatin1("serein-sticker-catalog") };
	for (auto i = 0; i != 1001; ++i) {
		many.sets.push_back({
			.shortName = QString::fromLatin1("set_") + QString::number(i),
			.type = QString::fromLatin1("stickers"),
		});
	}
	Require(!ParseStickerCatalogFile(SerializeStickerCatalogFile(many)),
		"too many sticker sets accepted");
}
