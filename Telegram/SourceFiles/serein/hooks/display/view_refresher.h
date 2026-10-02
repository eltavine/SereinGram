#pragma once

#include <gsl/pointers>

namespace Data {
class Session;
} // namespace Data
namespace Main {
class Session;
} // namespace Main

namespace Serein {

class ViewRefresher final {
public:
	static void Attach(not_null<Main::Session*> session);
	static void Refresh(Data::Session &data);

};

} // namespace Serein
