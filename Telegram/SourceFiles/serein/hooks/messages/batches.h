#pragma once

#include <cstddef>
#include <vector>

namespace Serein::Hooks {

inline constexpr auto kServerIdsLimit = std::size_t(100);

template <typename Ids, typename Send>
[[nodiscard]] bool SplitIds(const Ids &ids, Send &&send) {
	const auto count = std::size_t(ids.size());
	if (count <= kServerIdsLimit) {
		return false;
	}
	using Size = decltype(ids.size());
	for (auto i = std::size_t(); i < count; i += kServerIdsLimit) {
		send(ids.mid(Size(i), Size(kServerIdsLimit)));
	}
	return true;
}

template <typename Draft>
[[nodiscard]] std::vector<Draft> ForwardParts(const Draft &draft) {
	auto result = std::vector<Draft>();
	const auto &items = draft.items;
	if (items.size() <= kServerIdsLimit) {
		return result;
	}
	auto part = Draft{ .options = draft.options };
	for (auto i = std::size_t(); i != items.size();) {
		auto end = i + 1;
		if (const auto group = items[i]->groupId()) {
			while (end != items.size() && items[end]->groupId() == group) {
				++end;
			}
		}
		if (!part.items.empty()
			&& part.items.size() + (end - i) > kServerIdsLimit) {
			result.push_back(std::move(part));
			part = Draft{ .options = draft.options };
		}
		part.items.insert(
			part.items.end(),
			items.begin() + std::ptrdiff_t(i),
			items.begin() + std::ptrdiff_t(end));
		i = end;
	}
	result.push_back(std::move(part));
	return result;
}

template <typename Draft, typename Done, typename Send>
[[nodiscard]] bool SplitForward(const Draft &draft, Done &done, Send &&send) {
	auto parts = ForwardParts(draft);
	if (parts.empty()) {
		return false;
	}
	const auto last = parts.size() - 1;
	for (auto i = std::size_t(); i != last; ++i) {
		send(std::move(parts[i]), Done());
	}
	send(std::move(parts[last]), std::move(done));
	return true;
}

} // namespace Serein::Hooks
