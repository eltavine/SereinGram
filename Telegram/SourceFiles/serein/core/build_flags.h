#pragma once

namespace Serein {

#ifdef SEREIN_SYSTEM_PACKAGE
inline constexpr auto kSystemPackage = true;
#else // SEREIN_SYSTEM_PACKAGE
inline constexpr auto kSystemPackage = false;
#endif // SEREIN_SYSTEM_PACKAGE

} // namespace Serein
