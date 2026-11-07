#pragma once

#include "base/basic_types.h"

#include <QtCore/QString>

#include <memory>
#include <vector>

namespace Serein::Purge {

struct Progress {
	int deleted = 0;
	int total = 0;
};

enum class Outcome {
	Done,
	Stopped,
	Failed,
};

struct Result {
	Outcome outcome = Outcome::Done;
	int deleted = 0;
	QString error;
};

class Gateway {
public:
	virtual ~Gateway() = default;

	virtual void page(
		int identity,
		qint64 before,
		qint64 offsetId,
		Fn<void(std::vector<qint64>)> done,
		Fn<void(QString)> fail) = 0;
	virtual void erase(int identity, const std::vector<qint64> &ids) = 0;

};

class Job final : public std::enable_shared_from_this<Job> {
public:
	Job(
		std::shared_ptr<Gateway> gateway,
		std::vector<int> identities,
		qint64 before,
		int total);

	void start(Fn<void(Progress)> progress, Fn<void(Result)> finished);
	void stop();

private:
	void next();
	void received(std::vector<qint64> ids);
	void finish(Outcome outcome, QString error = QString());

	const std::shared_ptr<Gateway> _gateway;
	const std::vector<int> _identities;
	const qint64 _before = 0;
	const int _total = 0;
	Fn<void(Progress)> _progress;
	Fn<void(Result)> _finished;
	std::size_t _index = 0;
	qint64 _offsetId = 0;
	int _deleted = 0;
	bool _stopped = false;
	bool _done = false;

};

} // namespace Serein::Purge
