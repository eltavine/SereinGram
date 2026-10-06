#include "serein/features/purge/model/job.h"

#include <algorithm>

namespace Serein::Purge {

Job::Job(
	std::shared_ptr<Gateway> gateway,
	std::vector<int> identities,
	qint64 before,
	int total)
: _gateway(std::move(gateway))
, _identities(std::move(identities))
, _before(before)
, _total(total) {
}

void Job::start(Fn<void(Progress)> progress, Fn<void(Result)> finished) {
	_progress = std::move(progress);
	_finished = std::move(finished);
	next();
}

void Job::stop() {
	_stopped = true;
}

void Job::next() {
	if (_stopped) {
		finish(Outcome::Stopped);
		return;
	} else if (_index >= _identities.size()) {
		finish(Outcome::Done);
		return;
	}
	const auto weak = weak_from_this();
	_gateway->page(_identities[_index], _before, _offsetId, [=](
			std::vector<qint64> ids) {
		if (const auto strong = weak.lock()) {
			strong->received(std::move(ids));
		}
	}, [=](QString error) {
		if (const auto strong = weak.lock()) {
			strong->finish(Outcome::Failed, std::move(error));
		}
	});
}

void Job::received(std::vector<qint64> ids) {
	if (_done) {
		return;
	} else if (_stopped) {
		finish(Outcome::Stopped);
		return;
	}
	const auto lowest = ids.empty()
		? qint64()
		: *std::min_element(begin(ids), end(ids));
	if (ids.empty() || (_offsetId && lowest >= _offsetId)) {
		++_index;
		_offsetId = 0;
		next();
		return;
	}
	_gateway->erase(_identities[_index], ids);
	_deleted += int(ids.size());
	_offsetId = lowest;
	if (_progress) {
		_progress({ .deleted = _deleted, .total = std::max(_total, _deleted) });
	}
	next();
}

void Job::finish(Outcome outcome, QString error) {
	if (_done) {
		return;
	}
	_done = true;
	if (_finished) {
		_finished({
			.outcome = outcome,
			.deleted = _deleted,
			.error = std::move(error),
		});
	}
}

} // namespace Serein::Purge
