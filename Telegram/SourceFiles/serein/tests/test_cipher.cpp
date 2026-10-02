#include "serein/adapters/openssl/aes_gcm_cipher.h"
#include "serein/tests/require.h"

#include <doctest/doctest.h>
#include <iostream>

TEST_CASE("Cipher") {
	using Serein::Adapters::AesGcmCipher;
	const auto secret = QByteArray(256, '\x5a');
	Require(!AesGcmCipher::FromSecret(QByteArray(8, 'x'), "ctx"), "short secrets are refused");
	const auto cipher = AesGcmCipher::FromSecret(secret, "serein-history-v1");
	const auto other = AesGcmCipher::FromSecret(secret, "serein-other-v1");
	Require(cipher && other, "ciphers are created");

	const auto plain = QByteArray("{\"text\":\"hello\"}");
	const auto sealed = cipher->encrypt(plain);
	Require(sealed.size() == plain.size() + 29 && !sealed.contains(plain),
		"payload is sealed with a nonce and a tag");
	Require(sealed != cipher->encrypt(plain), "every encryption uses a new nonce");
	Require(cipher->decrypt(sealed) == plain, "sealed payload opens");
	Require(cipher->decrypt(cipher->encrypt(QByteArray())) == QByteArray(),
		"empty payloads round-trip");
	Require(!other->decrypt(sealed), "another context cannot open the payload");

	auto tampered = sealed;
	tampered[tampered.size() / 2] = char(tampered[tampered.size() / 2] ^ 1);
	Require(!cipher->decrypt(tampered), "tampered payloads are rejected");
	Require(!cipher->decrypt(sealed.left(10)), "truncated payloads are rejected");
	std::cout << "PASS: Serein AES-GCM cipher" << std::endl;
}
