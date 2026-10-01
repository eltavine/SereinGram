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
	auto devicePrefs = Prefs();
	auto account = Serein::Options(prefs, Serein::Scope::Account);
	auto device = Serein::Options(devicePrefs, Serein::Scope::Device);
	auto policy = Read(account, device);
	for (const auto activity : { Activity::ReadReceipt, Activity::StoryView,
			Activity::Online, Activity::Typing, Activity::ViewIncrement }) {
		Require(Allows(policy, activity), "ghost mode is off by default");
	}
	Require(!OfflineAfterSending(policy)
		&& !ScheduleOutgoing(policy)
		&& !SendSilently(policy),
		"no ghost side effects by default");

	Require(account.Set(kGhostMode, true), "ghost mode turns on");
	policy = Read(account, device);
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
		&& account.Set(kGhostUseScheduledMessages, true)
		&& account.Set(kGhostSendSilently, true), "ghost details change");
	policy = Read(account, device);
	Require(Allows(policy, Activity::Typing), "typing can stay visible");
	Require(!Allows(policy, Activity::ViewIncrement), "view increments can be hidden");
	Require(ScheduleOutgoing(policy), "scheduled sending can be enabled");
	Require(SendSilently(policy), "silent sending can be enabled");

	Require(account.Set(kGhostMode, false), "ghost mode turns off");
	Require(Allows(Read(account, device), Activity::ViewIncrement)
		&& !SendSilently(Read(account, device)),
		"details do not apply while ghost mode is off");

	Require(device.Set(kGhostAllAccounts, true)
		&& Enabled(account, device)
		&& !Allows(Read(account, device), Activity::ReadReceipt),
		"ghost mode on all accounts applies without the account switch");
	Require(SetEnabled(account, device, false)
		&& !account.Get(kGhostMode)
		&& !device.Get(kGhostAllAccounts),
		"turning ghost mode off clears both switches");
	Require(SetEnabled(account, device, true)
		&& account.Get(kGhostMode)
		&& !device.Get(kGhostAllAccounts),
		"turning ghost mode on only changes the account");
	std::cout << "PASS: Serein ghost policy" << std::endl;
}
