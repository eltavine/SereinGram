#include "serein/chats/reading_positions.h"

#include "serein/chats/options.h"

#include <QtCore/QStringList>

#include <algorithm>
#include <optional>
#include <utility>
#include <vector>

namespace Serein::Chats {
namespace {

using Position = std::pair<quint64, qint64>;

[[nodiscard]] std::optional<std::vector<Position>> Parse(const QString &value) {
	auto result = std::vector<Position>();
	if (value.isEmpty()) {
		return result;
	}
	for (const auto &part : value.split(u',')) {
		const auto fields = part.split(u':');
		auto peerValid = false;
		auto messageValid = false;
		const auto peer = (fields.size() == 2)
			? fields[0].toULongLong(&peerValid)
			: 0;
		const auto message = (fields.size() == 2)
			? fields[1].toLongLong(&messageValid)
			: 0;
		if (!peerValid
			|| !messageValid
			|| !peer
			|| message <= 0
			|| part != QString::number(peer) + u':' + QString::number(message)
			|| std::any_of(result.begin(), result.end(), [&](
					const Position &entry) {
				return entry.first == peer;
			})) {
			return std::nullopt;
		}
		result.emplace_back(peer, message);
	}
	if (int(result.size()) > kReadingPositionsLimit) {
		return std::nullopt;
	}
	return result;
}

} // namespace

bool ValidReadingPositions(const QString &value) {
	return Parse(value).has_value();
}

qint64 FindReadingPosition(const QString &value, quint64 peer) {
	const auto positions = Parse(value).value_or(std::vector<Position>());
	const auto i = std::find_if(positions.begin(), positions.end(), [&](
			const Position &entry) {
		return entry.first == peer;
	});
	return (i != positions.end()) ? i->second : 0;
}

QString SetReadingPosition(const QString &value, quint64 peer, qint64 message) {
	auto positions = Parse(value).value_or(std::vector<Position>());
	positions.erase(std::remove_if(positions.begin(), positions.end(), [&](
			const Position &entry) {
		return entry.first == peer;
	}), positions.end());
	if (peer && message > 0) {
		positions.insert(positions.begin(), Position(peer, message));
	}
	if (int(positions.size()) > kReadingPositionsLimit) {
		positions.resize(kReadingPositionsLimit);
	}
	auto parts = QStringList();
	for (const auto &[entryPeer, entryMessage] : positions) {
		parts.push_back(QString::number(entryPeer)
			+ u':'
			+ QString::number(entryMessage));
	}
	return parts.join(u',');
}

} // namespace Serein::Chats
