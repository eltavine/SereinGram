#include "serein/core/session_lifetime.h"

#include <doctest/doctest.h>
#include <rpl/variable.h>

#include <map>
#include <memory>
#include <stdexcept>

namespace {

class SessionProbe;

class StateRegistry {
public:
	void ensure(SessionProbe *session);
	rpl::producer<SessionProbe*> sessionValue() const;

	rpl::variable<SessionProbe*> published = nullptr;
	std::map<SessionProbe*, std::unique_ptr<rpl::lifetime>> states;
	int cleanups = 0;

};

struct EarlyRegistration {
	EarlyRegistration(
		StateRegistry &registry,
		SessionProbe *session,
		bool registerEarly);
};

class SessionProbe {
public:
	explicit SessionProbe(
		StateRegistry &registry,
		bool registerEarly = true);

	rpl::lifetime &lifetime();
	int lifetimeCalls() const;

private:
	bool _constructed = false;
	int _lifetimeCalls = 0;
	EarlyRegistration _registration;
	rpl::lifetime _lifetime;

};

void StateRegistry::ensure(SessionProbe *session) {
	if (states.contains(session)) {
		return;
	}
	const auto state = states.emplace(
		session,
		std::make_unique<rpl::lifetime>()).first;
	Serein::details::BindSessionCleanup(
		gsl::not_null{ session },
		sessionValue(),
		[=, this] {
			++cleanups;
			states.erase(session);
		},
		*state->second);
}

rpl::producer<SessionProbe*> StateRegistry::sessionValue() const {
	return published.value();
}

EarlyRegistration::EarlyRegistration(
		StateRegistry &registry,
		SessionProbe *session,
		bool registerEarly) {
	if (registerEarly) {
		registry.ensure(session);
	}
}

SessionProbe::SessionProbe(StateRegistry &registry, bool registerEarly)
: _registration(registry, this, registerEarly) {
	_constructed = true;
}

rpl::lifetime &SessionProbe::lifetime() {
	if (!_constructed) {
		throw std::logic_error("Session lifetime accessed during construction");
	}
	++_lifetimeCalls;
	return _lifetime;
}

int SessionProbe::lifetimeCalls() const {
	return _lifetimeCalls;
}

} // namespace

TEST_CASE("Session cleanup waits for construction and runs once") {
	auto registry = StateRegistry();
	auto session = std::make_unique<SessionProbe>(registry);
	CHECK(registry.states.size() == 1);
	CHECK(session->lifetimeCalls() == 0);
	CHECK(registry.cleanups == 0);

	registry.published = session.get();
	CHECK(session->lifetimeCalls() == 1);
	registry.ensure(session.get());
	registry.published = nullptr;
	registry.published = session.get();
	CHECK(session->lifetimeCalls() == 1);

	session.reset();
	CHECK(registry.cleanups == 1);
	CHECK(registry.states.empty());
}

TEST_CASE("Session cleanup also binds to an already published session") {
	auto registry = StateRegistry();
	auto session = std::make_unique<SessionProbe>(registry, false);
	registry.published = session.get();
	registry.ensure(session.get());
	CHECK(session->lifetimeCalls() == 1);
	CHECK(registry.states.size() == 1);

	session.reset();
	CHECK(registry.cleanups == 1);
	CHECK(registry.states.empty());
}

TEST_CASE("Session cleanup ignores publication of another session") {
	auto registry = StateRegistry();
	auto session = std::make_unique<SessionProbe>(registry);
	auto other = std::make_unique<SessionProbe>(registry, false);
	registry.published = other.get();
	CHECK(session->lifetimeCalls() == 0);
	CHECK(other->lifetimeCalls() == 0);
	registry.published = nullptr;
	CHECK(session->lifetimeCalls() == 0);

	registry.published = session.get();
	CHECK(session->lifetimeCalls() == 1);
	CHECK(other->lifetimeCalls() == 0);
	session.reset();
	CHECK(registry.cleanups == 1);
	CHECK(registry.states.empty());
}

TEST_CASE("Removing early state cancels its pending session cleanup") {
	auto registry = StateRegistry();
	auto session = std::make_unique<SessionProbe>(registry);
	registry.states.erase(session.get());
	registry.published = session.get();
	CHECK(session->lifetimeCalls() == 0);

	session.reset();
	CHECK(registry.cleanups == 0);
	CHECK(registry.states.empty());
}

TEST_CASE("Session cleanup keeps different session states independent") {
	auto registry = StateRegistry();
	auto first = std::make_unique<SessionProbe>(registry);
	auto second = std::make_unique<SessionProbe>(registry);
	CHECK(registry.states.size() == 2);
	registry.published = first.get();
	CHECK(first->lifetimeCalls() == 1);
	CHECK(second->lifetimeCalls() == 0);
	registry.published = second.get();
	CHECK(first->lifetimeCalls() == 1);
	CHECK(second->lifetimeCalls() == 1);

	first.reset();
	CHECK(registry.cleanups == 1);
	CHECK(registry.states.size() == 1);
	CHECK(registry.states.contains(second.get()));
	second.reset();
	CHECK(registry.cleanups == 2);
	CHECK(registry.states.empty());
}
