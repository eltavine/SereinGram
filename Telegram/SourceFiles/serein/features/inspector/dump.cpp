#include "serein/features/inspector/dump.h"

#include "mtproto/details/mtproto_dump_to_text.h"

#include <algorithm>

namespace Serein::Inspector {
namespace {

constexpr auto kPrime = int(sizeof(mtpPrime));
constexpr auto kEnvelope = 4;

} // namespace

std::optional<Node> ReadSerialized(const QByteArray &primes) {
	if (primes.isEmpty() || (primes.size() % kPrime)) {
		return std::nullopt;
	}
	const auto body = reinterpret_cast<const mtpPrime*>(primes.constData());
	const auto count = primes.size() / kPrime;
	auto envelope = mtpBuffer(count + kEnvelope, mtpPrime(0));
	envelope[kEnvelope - 1] = mtpPrime(primes.size());
	std::copy(body, body + count, envelope.begin() + kEnvelope);
	auto from = envelope.constData();
	const auto till = from + envelope.size();
	return ParseDump(MTP::details::DumpToText(from, till));
}

} // namespace Serein::Inspector
