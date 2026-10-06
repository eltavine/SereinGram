#pragma once

#include "serein/features/inspector/model/tl_tree.h"
#include "mtproto/core_types.h"

#include <QtCore/QByteArray>

#include <optional>

namespace Serein::Inspector {

[[nodiscard]] std::optional<Node> ReadSerialized(const QByteArray &primes);

template <typename Type>
[[nodiscard]] QByteArray Serialize(const Type &value) {
	auto buffer = mtpBuffer();
	value.write(buffer);
	return QByteArray(
		reinterpret_cast<const char*>(buffer.constData()),
		int(buffer.size() * sizeof(mtpPrime)));
}

} // namespace Serein::Inspector
