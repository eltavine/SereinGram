#pragma once

#include <QtCore/QByteArray>

#include <string_view>

namespace Serein {

class RawPrefs {
public:
	virtual ~RawPrefs() = default;
	[[nodiscard]] virtual QByteArray read(std::string_view key) = 0;
	virtual void write(std::string_view key, const QByteArray &value) = 0;
	virtual void clear(std::string_view key) = 0;

};

} // namespace Serein
