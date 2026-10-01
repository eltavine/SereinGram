#include "serein/hooks/messages/batches.h"

#include <QtCore/QList>

#include <functional>
#include <optional>
#include <stdexcept>
#include <vector>

namespace {

struct FakeItem {
	std::optional<int> group;

	[[nodiscard]] std::optional<int> groupId() const {
		return group;
	}
};

struct FakeDraft {
	std::vector<const FakeItem*> items;
	int options = 0;
};

void Require(bool value, const char *message) {
	if (!value) {
		throw std::runtime_error(message);
	}
}

[[nodiscard]] std::vector<std::size_t> Sizes(const std::vector<FakeDraft> &parts) {
	auto result = std::vector<std::size_t>();
	for (const auto &part : parts) {
		result.push_back(part.items.size());
	}
	return result;
}

} // namespace

void TestBatches() {
	using namespace Serein::Hooks;
	auto singles = std::vector<FakeItem>(205);
	auto draft = FakeDraft{ .options = 2 };
	for (const auto &item : singles) {
		draft.items.push_back(&item);
	}
	auto small = draft;
	small.items.resize(100);
	Require(ForwardParts(small).empty(), "a full request is split");
	const auto parts = ForwardParts(draft);
	Require(Sizes(parts) == std::vector<std::size_t>{ 100, 100, 5 },
		"singles are not split by 100");
	Require(parts.back().options == 2, "forward options are not kept");

	auto album = std::vector<FakeItem>(10, FakeItem{ .group = 7 });
	auto mixed = FakeDraft();
	for (auto i = 0; i != 95; ++i) {
		mixed.items.push_back(&singles[i]);
	}
	for (const auto &item : album) {
		mixed.items.push_back(&item);
	}
	Require(Sizes(ForwardParts(mixed)) == std::vector<std::size_t>{ 95, 10 },
		"an album is split across requests");

	auto ids = QList<int>();
	for (auto i = 0; i != 250; ++i) {
		ids.push_back(i);
	}
	auto batches = std::vector<qsizetype>();
	Require(SplitIds(ids, [&](const QList<int> &part) {
		batches.push_back(part.size());
	}), "large id list is not split");
	Require(batches == std::vector<qsizetype>{ 100, 100, 50 },
		"ids are not split by 100");
	Require(!SplitIds(ids.mid(0, 100), [](const QList<int> &) {}),
		"a full id list is split");

	auto calls = std::vector<bool>();
	auto done = std::function<void()>([] {});
	Require(SplitForward(draft, done, [&](FakeDraft &&, std::function<void()> &&part) {
		calls.push_back(bool(part));
	}), "large forward is not split");
	Require(calls == std::vector<bool>{ false, false, true },
		"the completion callback is not passed to the last part only");
}
