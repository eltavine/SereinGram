#pragma once

#include "data/data_messages.h"

#include <memory>

class History;

namespace Serein::HistoryFeature::Viewer {

struct Query {
	PeerId peer = 0;
	MsgId messageId = 0;

	[[nodiscard]] bool versions() const {
		return messageId != 0;
	}

	friend inline bool operator==(const Query &, const Query &) = default;
};

class Source {
public:
	virtual ~Source() = default;

	[[nodiscard]] virtual rpl::producer<Data::MessagesSlice> slice(
		Data::MessagePosition around,
		int limitBefore,
		int limitAfter) = 0;
	[[nodiscard]] virtual rpl::producer<int> countValue() const = 0;

};

[[nodiscard]] std::unique_ptr<Source> MakeSource(
	not_null<::History*> history,
	const Query &query);

} // namespace Serein::HistoryFeature::Viewer
