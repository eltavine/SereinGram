#include "serein/features/purge/model/job.h"
#include "serein/features/purge/model/plan.h"
#include "serein/tests/require.h"

#include <doctest/doctest.h>

#include <optional>
#include <utility>

namespace {

using namespace Serein::Purge;

struct Request {
	int identity = 0;
	qint64 offsetId = 0;
};

class FakeGateway final : public Gateway {
public:
	explicit FakeGateway(std::vector<std::vector<qint64>> messages)
	: _messages(std::move(messages)) {
	}

	void page(
			int identity,
			qint64 before,
			qint64 offsetId,
			Fn<void(std::vector<qint64>)> done,
			Fn<void(QString)> fail) override {
		requests.push_back({ identity, offsetId });
		lastBefore = before;
		if (!failure.isEmpty() && int(requests.size()) > failAfter) {
			fail(failure);
			return;
		}
		auto result = std::vector<qint64>();
		for (const auto id : _messages[identity]) {
			const auto fits = stuck || !offsetId || (id < offsetId);
			if (fits && int(result.size()) < kPage) {
				result.push_back(id);
			}
		}
		if (deferred) {
			pending = [=] { done(result); };
		} else {
			done(result);
		}
	}

	void erase(int identity, const std::vector<qint64> &ids) override {
		erased.emplace_back(identity, ids);
	}

	static constexpr auto kPage = 3;

	std::vector<Request> requests;
	std::vector<std::pair<int, std::vector<qint64>>> erased;
	Fn<void()> pending;
	QString failure;
	int failAfter = 0;
	qint64 lastBefore = 0;
	bool deferred = false;
	bool stuck = false;

private:
	std::vector<std::vector<qint64>> _messages;

};

struct Run {
	std::vector<Progress> progress;
	std::optional<Result> result;
};

[[nodiscard]] std::shared_ptr<Job> Start(
		std::shared_ptr<FakeGateway> gateway,
		std::vector<int> identities,
		int total,
		Run &run) {
	auto job = std::make_shared<Job>(gateway, identities, 500, total);
	job->start([&](Progress progress) {
		run.progress.push_back(progress);
	}, [&](Result result) {
		run.result = result;
	});
	return job;
}

} // namespace

TEST_CASE("PurgeCutoffs") {
	constexpr auto kNow = qint64(10'000'000);
	constexpr auto kDay = qint64(86'400);
	Require(Cutoff(Age::All, kNow, 0) == 0, "everything");
	Require(Cutoff(Age::Day, kNow, 0) == kNow - kDay, "a day");
	Require(Cutoff(Age::Week, kNow, 0) == kNow - 7 * kDay, "a week");
	Require(Cutoff(Age::Month, kNow, 0) == kNow - 30 * kDay, "a month");
	Require(Cutoff(Age::Year, kNow, 0) == kNow - 365 * kDay, "a year");
	Require(Cutoff(Age::Custom, kNow, 1234) == 1234, "chosen date");
	Require(Cutoff(Age::Custom, kNow, 0) == 0, "no date chosen yet");
}

TEST_CASE("PurgeJobDeletesPageByPage") {
	const auto gateway = std::make_shared<FakeGateway>(
		std::vector<std::vector<qint64>>{
			{ 10, 9, 8, 7, 6 },
			{ 20 },
		});
	auto run = Run();
	const auto job = Start(gateway, { 0, 1 }, 4, run);
	Require(run.result.has_value(), "finished");
	Require(run.result->outcome == Outcome::Done, "done");
	Require(run.result->deleted == 6, "every message deleted");
	Require(gateway->lastBefore == 500, "cutoff passed to the search");
	Require(gateway->erased.size() == 3, "one erase per page");
	Require(
		gateway->erased[0].second == std::vector<qint64>{ 10, 9, 8 },
		"first page");
	Require(gateway->erased[2].first == 1, "second identity last");
	Require(gateway->requests.size() == 5, "pages until each is empty");
	Require(gateway->requests[1].offsetId == 8, "offset is the lowest id");
	Require(gateway->requests[3].offsetId == 0, "offset resets per identity");
	Require(run.progress.size() == 3, "progress per page");
	Require(
		run.progress.back().deleted == 6 && run.progress.back().total == 6,
		"total grows past the estimate");
}

TEST_CASE("PurgeJobStopsBeforeTheNextPage") {
	const auto gateway = std::make_shared<FakeGateway>(
		std::vector<std::vector<qint64>>{ { 3, 2, 1 } });
	gateway->deferred = true;
	auto run = Run();
	const auto job = Start(gateway, { 0 }, 3, run);
	Require(!run.result.has_value(), "waiting for the page");
	job->stop();
	gateway->pending();
	Require(run.result.has_value(), "finished after the page");
	Require(run.result->outcome == Outcome::Stopped, "stopped");
	Require(gateway->erased.empty(), "the late page is not deleted");
}

TEST_CASE("PurgeJobReportsFailures") {
	const auto gateway = std::make_shared<FakeGateway>(
		std::vector<std::vector<qint64>>{ { 6, 5, 4, 3, 2, 1 } });
	gateway->failure = u"FLOOD_WAIT_X"_q;
	gateway->failAfter = 1;
	auto run = Run();
	const auto job = Start(gateway, { 0 }, 6, run);
	Require(run.result.has_value(), "finished");
	Require(run.result->outcome == Outcome::Failed, "failed");
	Require(run.result->error == u"FLOOD_WAIT_X"_q, "error kept");
	Require(run.result->deleted == 3, "work before the error counted");
}

TEST_CASE("PurgeJobLeavesStuckIdentities") {
	const auto gateway = std::make_shared<FakeGateway>(
		std::vector<std::vector<qint64>>{ { 9, 8 }, { 5 } });
	gateway->stuck = true;
	auto run = Run();
	const auto job = Start(gateway, { 0, 1 }, 3, run);
	Require(run.result.has_value(), "finished");
	Require(run.result->outcome == Outcome::Done, "done");
	Require(run.result->deleted == 3, "each page deleted once");
	Require(gateway->erased.size() == 2, "no repeated pages");
}
