#include "serein/features/history/wire.h"

namespace Serein::HistoryFeature {
namespace {

constexpr auto kPrimeSize = int(sizeof(mtpPrime));

} // namespace

int WireLayer() {
	return int(MTP::details::kCurrentLayer);
}

QByteArray SerializeMessage(const MTPMessage &message) {
	auto buffer = mtpBuffer();
	message.write(buffer);
	return QByteArray(
		reinterpret_cast<const char*>(buffer.constData()),
		int(buffer.size()) * kPrimeSize);
}

std::optional<MTPMessage> ParseMessage(const QByteArray &bytes) {
	if (bytes.isEmpty() || (bytes.size() % kPrimeSize)) {
		return std::nullopt;
	}
	auto from = reinterpret_cast<const mtpPrime*>(bytes.constData());
	const auto till = from + (bytes.size() / kPrimeSize);
	auto result = MTPMessage();
	if (!result.read(from, till) || from != till) {
		return std::nullopt;
	}
	return result;
}

} // namespace Serein::HistoryFeature
