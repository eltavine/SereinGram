#include "serein/features/history/viewer/source.h"

#include "serein/features/history/backend.h"
#include "serein/features/history/capture.h"
#include "serein/features/history/deleted_marks.h"
#include "serein/features/history/model/window.h"
#include "serein/features/history/viewer/items.h"
#include "base/weak_ptr.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "history/history.h"
#include "history/history_item.h"
#include "main/main_session.h"

namespace Serein::HistoryFeature::Viewer {
namespace {

using Kind = Serein::History::RecordKind;

class RecordsSource : public Source, public base::has_weak_ptr {
public:
	RecordsSource(not_null<::History*> history, Query query)
	: _items(history)
	, _query(query) {
	}

	rpl::producer<Data::MessagesSlice> slice(
			Data::MessagePosition around,
			int limitBefore,
			int limitAfter) override {
		const auto peerId = qint64(_query.peer.value);
		return [=](auto consumer) {
			auto lifetime = rpl::lifetime();
			const auto push = crl::guard(this, [=] {
				consumer.put_next(load(around, limitBefore, limitAfter));
			});
			RecordsChanged(
				&_items.history()->session()
			) | rpl::filter([=](qint64 changed) {
				return !changed || (changed == peerId);
			}) | rpl::on_next([=](qint64) {
				push();
			}, lifetime);
			push();
			return lifetime;
		};
	}

	rpl::producer<int> countValue() const override {
		return _count.value();
	}

protected:
	struct Loaded {
		std::vector<ItemEntry> entries;
		std::optional<Ports::RecordKey> nearest;
		int skippedBefore = 0;
		int skippedAfter = 0;
	};

	[[nodiscard]] virtual Loaded entries(
		std::optional<Ports::RecordKey> around,
		int limitBefore,
		int limitAfter) = 0;

	[[nodiscard]] Ports::HistoryStore *store() const {
		return StoreFor(&_items.history()->session());
	}

	[[nodiscard]] const Query &query() const {
		return _query;
	}

	[[nodiscard]] not_null<::History*> history() const {
		return _items.history();
	}

private:
	[[nodiscard]] Data::MessagesSlice load(
			Data::MessagePosition around,
			int limitBefore,
			int limitAfter) {
		auto loaded = entries(
			_items.keyOf(around.fullId),
			limitBefore,
			limitAfter);
		auto result = Data::MessagesSlice();
		auto keys = std::vector<Ports::RecordKey>();
		auto items = std::vector<not_null<HistoryItem*>>();
		for (const auto &entry : loaded.entries) {
			const auto item = _items.ensure(entry);
			const auto key = Ports::KeyOf(entry.record);
			if (loaded.nearest == key) {
				result.nearestToAround = item->fullId();
			}
			keys.push_back(key);
			items.push_back(item);
		}
		ranges::sort(items, std::less<>(), &HistoryItem::position);
		result.ids = items
			| ranges::views::transform(&HistoryItem::fullId)
			| ranges::to_vector;
		result.skippedBefore = loaded.skippedBefore;
		result.skippedAfter = loaded.skippedAfter;
		result.fullCount = loaded.skippedBefore
			+ int(items.size())
			+ loaded.skippedAfter;
		_count = *result.fullCount;
		scheduleRetain(std::move(keys));
		return result;
	}

	void scheduleRetain(std::vector<Ports::RecordKey> keys) {
		_retained = std::move(keys);
		if (_retainScheduled) {
			return;
		}
		_retainScheduled = true;
		crl::on_main(this, [=] {
			_retainScheduled = false;
			_items.retain(_retained);
		});
	}

	RestoredItems _items;
	const Query _query;
	rpl::variable<int> _count = -1;
	std::vector<Ports::RecordKey> _retained;
	bool _retainScheduled = false;

};

class DeletedSource final : public RecordsSource {
public:
	using RecordsSource::RecordsSource;

private:
	Loaded entries(
			std::optional<Ports::RecordKey> around,
			int limitBefore,
			int limitAfter) override {
		const auto store = this->store();
		if (!store) {
			return {};
		}
		const auto window = LoadWindow(*store, {
			.peerId = qint64(query().peer.value),
			.kind = Kind::Deleted,
		}, {
			.around = around,
			.limitBefore = limitBefore,
			.limitAfter = limitAfter,
		});
		auto result = Loaded{
			.nearest = window.nearest,
			.skippedBefore = window.skippedBefore,
			.skippedAfter = window.skippedAfter,
		};
		for (const auto &record : window.records) {
			result.entries.push_back({
				.record = record,
				.date = TimeId(record.date),
				.deleted = true,
			});
		}
		return result;
	}

};

class VersionsSource final : public RecordsSource {
public:
	using RecordsSource::RecordsSource;

private:
	Loaded entries(std::optional<Ports::RecordKey>, int, int) override {
		const auto store = this->store();
		if (!store) {
			return {};
		}
		const auto records = store->records({
			.peerId = qint64(query().peer.value),
			.messageId = query().messageId.bare,
		});
		auto result = Loaded();
		auto became = TimeId();
		for (const auto &record : records) {
			const auto first = result.entries.empty();
			result.entries.push_back({
				.record = record,
				.date = first ? TimeId(record.date) : became,
				.version = true,
				.deleted = (record.kind == Kind::Deleted),
				.edited = !first,
			});
			became = TimeId(record.recordedAt);
		}
		if (auto current = this->current(records, became)) {
			result.entries.push_back(std::move(*current));
		}
		if (!result.entries.empty()) {
			result.nearest = Ports::KeyOf(result.entries.back().record);
		}
		return result;
	}

	[[nodiscard]] std::optional<ItemEntry> current(
			const std::vector<Serein::History::Record> &records,
			TimeId became) const {
		if (records.empty() || records.back().kind == Kind::Deleted) {
			return std::nullopt;
		}
		const auto live = history()->owner().message(
			query().peer,
			query().messageId);
		if (!live || DeletedInPlace(live)) {
			return std::nullopt;
		}
		return ItemEntry{
			.record = RecordFromSnapshot(
				Kind::Edited,
				TakeSnapshot(live),
				records.back().revision + 1,
				0),
			.date = became,
			.version = true,
			.edited = true,
		};
	}

};

} // namespace

std::unique_ptr<Source> MakeSource(
		not_null<::History*> history,
		const Query &query) {
	if (query.versions()) {
		return std::make_unique<VersionsSource>(history, query);
	}
	return std::make_unique<DeletedSource>(history, query);
}

} // namespace Serein::HistoryFeature::Viewer
