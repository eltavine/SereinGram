#include "serein/services/credentials.h"
#include "base/basic_types.h"

#include <map>
#include <stdexcept>

namespace {

auto Stored = std::map<QString, QByteArray>();
auto Calls = 0;

void Require(bool value, const char *message) {
	if (!value) {
		throw std::runtime_error(message);
	}
}

} // namespace

namespace Serein::Ports::SecretStore {

// The core tests link this in-memory store instead of a platform adapter.
CredentialResult Read(const QString &account) {
	++Calls;
	const auto i = Stored.find(account);
	if (i == Stored.end()) {
		return { .error = CredentialError::Missing };
	}
	return { .secret = i->second };
}

CredentialError Write(const QString &account, const QByteArray &secret) {
	++Calls;
	Stored[account] = secret;
	return CredentialError::None;
}

CredentialError Remove(const QString &account) {
	++Calls;
	Stored.erase(account);
	return CredentialError::None;
}

} // namespace Serein::Ports::SecretStore

void TestCredentials() {
	using namespace Serein;
	const auto account = u"service:00000000-0000-0000-0000-000000000002"_q;
	Require(WriteCredential(account, "sk-local") == CredentialError::None,
		"valid secret rejected");
	const auto stored = ReadCredential(account);
	Require(stored.error == CredentialError::None
		&& stored.secret == "sk-local", "stored secret not returned");
	Require(DeleteCredential(account) == CredentialError::None,
		"stored secret not deleted");
	Require(ReadCredential(account).error == CredentialError::Missing,
		"deleted secret still readable");

	const auto longest = QString(256, QChar('a'));
	Require(WriteCredential(longest, QByteArray(4096, 'b'))
		== CredentialError::None, "secret at the size limits rejected");
	Require(ReadCredential(longest).secret.size() == 4096,
		"secret at the size limits truncated");

	const auto calls = Calls;
	const auto invalid = [](CredentialError error) {
		return error == CredentialError::Invalid;
	};
	Require(invalid(ReadCredential(QString()).error),
		"empty account accepted");
	Require(invalid(ReadCredential(longest + u"a"_q).error),
		"oversized account accepted");
	Require(invalid(ReadCredential(u"a"_q + QChar(0) + u"b"_q).error),
		"account with NUL accepted");
	Require(invalid(WriteCredential(account, QByteArray())),
		"empty secret accepted");
	Require(invalid(WriteCredential(account, QByteArray(4097, 'b'))),
		"oversized secret accepted");
	Require(invalid(WriteCredential(account, "first\nsecond")),
		"secret with a line feed accepted");
	Require(invalid(WriteCredential(account, "first\rsecond")),
		"secret with a carriage return accepted");
	Require(invalid(WriteCredential(account, QByteArray("a\0b", 3))),
		"secret with NUL accepted");
	Require(invalid(WriteCredential(QString(), "sk-local")),
		"secret written for an empty account");
	Require(invalid(DeleteCredential(QString())),
		"empty account deleted");
	Require(Calls == calls, "invalid input reached the secret store");
}
