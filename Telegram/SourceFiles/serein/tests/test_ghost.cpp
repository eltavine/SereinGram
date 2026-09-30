#include "serein/features/ghost/model/policy.h"

#include <iostream>
#include <map>
#include <stdexcept>
#include <string>

namespace {

void Require(bool value, const char *message) {
	if (!value) {
		throw std::runtime_error(message);
	}
}

class Prefs final : public Serein::RawPrefs {
public:
	QByteArray read(std::string_view key) override {
		const auto i = _values.find(std::string(key));
		return (i != _values.end()) ? i->second : QByteArray();
	}
	void write(std::string_view key, const QByteArray &value) override {
		_values[std::string(key)] = value;
	}
	void clear(std::string_view key) override {
		_values.erase(std::string(key));
	}

private:
	std::map<std::string, QByteArray> _values;
};

} // namespace

void TestGhost() {
	using namespace Serein::Ghost;
	auto prefs = Prefs();
	auto account = Serein::Options(prefs, Serein::Scope::Account);
	auto policy = Read(account);
	for (const auto activity : { Activity::ReadReceipt, Activity::StoryView,
			Activity::Online, Activity::Typing, Activity::ViewIncrement }) {
		Require(Allows(policy, activity), "ghost mode is off by default");
	}
	Require(!OfflineAfterSending(policy) && !ScheduleOutgoing(policy),
		"no ghost side effects by default");

	Require(account.Set(kGhostMode, true), "ghost mode turns on");
	policy = Read(account);
	Require(!Allows(policy, Activity::ReadReceipt)
		&& !Allows(policy, Activity::StoryView)
		&& !Allows(policy, Activity::Online)
		&& !Allows(policy, Activity::Typing),
		"ghost mode hides receipts, story views, online and typing");
	Require(Allows(policy, Activity::ViewIncrement),
		"view counters still increase unless asked");
	Require(OfflineAfterSending(policy), "ghost mode goes offline after sending");

	Require(account.Set(kGhostHideTyping, false)
		&& account.Set(kGhostHideViewIncrements, true)
		&& account.Set(kGhostUseScheduledMessages, true), "ghost details change");
	policy = Read(account);
	Require(Allows(policy, Activity::Typing), "typing can stay visible");
	Require(!Allows(policy, Activity::ViewIncrement), "view increments can be hidden");
	Require(ScheduleOutgoing(policy), "scheduled sending can be enabled");

	Require(account.Set(kGhostMode, false), "ghost mode turns off");
	Require(Allows(Read(account), Activity::ViewIncrement),
		"details do not apply while ghost mode is off");
	std::cout << "PASS: Serein ghost policy" << std::endl;
}
