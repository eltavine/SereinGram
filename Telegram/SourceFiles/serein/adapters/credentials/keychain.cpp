#include "serein/ports/credentials.h"

#include <Security/Security.h>

namespace Serein::Ports::SecretStore {
namespace {

class Query final {
public:
	explicit Query(const QString &account)
	: _value(CFDictionaryCreateMutable(
		nullptr,
		0,
		&kCFTypeDictionaryKeyCallBacks,
		&kCFTypeDictionaryValueCallBacks)) {
		const auto bytes = account.toUtf8();
		const auto key = CFStringCreateWithBytes(
			nullptr,
			reinterpret_cast<const UInt8*>(bytes.constData()),
			bytes.size(),
			kCFStringEncodingUTF8,
			false);
		CFDictionarySetValue(_value, kSecClass, kSecClassGenericPassword);
		CFDictionarySetValue(
			_value,
			kSecAttrService,
			CFSTR("SereinGram Services"));
		CFDictionarySetValue(_value, kSecAttrAccount, key);
		CFDictionarySetValue(
			_value,
			kSecAttrSynchronizable,
			kCFBooleanFalse);
		CFRelease(key);
	}
	Query(const Query &) = delete;
	Query &operator=(const Query &) = delete;
	~Query() {
		CFRelease(_value);
	}

	[[nodiscard]] CFMutableDictionaryRef get() const {
		return _value;
	}

private:
	CFMutableDictionaryRef _value = nullptr;

};

[[nodiscard]] CredentialError Error(OSStatus status) {
	return (status == errSecSuccess)
		? CredentialError::None
		: (status == errSecItemNotFound)
		? CredentialError::Missing
		: CredentialError::Denied;
}

} // namespace

CredentialResult Read(const QString &account) {
	auto query = Query(account);
	CFDictionarySetValue(query.get(), kSecReturnData, kCFBooleanTrue);
	CFDictionarySetValue(query.get(), kSecMatchLimit, kSecMatchLimitOne);
	CFDictionarySetValue(
		query.get(),
		kSecUseAuthenticationUI,
		kSecUseAuthenticationUIFail);
	auto result = CFTypeRef(nullptr);
	const auto status = SecItemCopyMatching(query.get(), &result);
	if (status != errSecSuccess) {
		return { .error = Error(status) };
	}
	if (!result || CFGetTypeID(result) != CFDataGetTypeID()) {
		if (result) {
			CFRelease(result);
		}
		return { .error = CredentialError::Invalid };
	}
	const auto data = static_cast<CFDataRef>(result);
	auto secret = QByteArray(
		reinterpret_cast<const char*>(CFDataGetBytePtr(data)),
		CFDataGetLength(data));
	CFRelease(result);
	return { .secret = std::move(secret) };
}

CredentialError Write(const QString &account, const QByteArray &secret) {
	auto query = Query(account);
	const auto data = CFDataCreate(
		nullptr,
		reinterpret_cast<const UInt8*>(secret.constData()),
		secret.size());
	const auto changes = CFDictionaryCreateMutable(
		nullptr,
		0,
		&kCFTypeDictionaryKeyCallBacks,
		&kCFTypeDictionaryValueCallBacks);
	CFDictionarySetValue(changes, kSecValueData, data);
	auto status = SecItemUpdate(query.get(), changes);
	if (status == errSecItemNotFound) {
		CFDictionarySetValue(query.get(), kSecValueData, data);
		status = SecItemAdd(query.get(), nullptr);
	}
	CFRelease(changes);
	CFRelease(data);
	return Error(status);
}

CredentialError Remove(const QString &account) {
	auto query = Query(account);
	const auto status = SecItemDelete(query.get());
	return (status == errSecItemNotFound)
		? CredentialError::None
		: Error(status);
}

} // namespace Serein::Ports::SecretStore
