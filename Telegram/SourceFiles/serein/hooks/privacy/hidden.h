#pragma once

namespace Main { class Account; class Session; }
class PeerData;

namespace Serein::Privacy {

[[nodiscard]] bool ChatHidden(gsl::not_null<const PeerData*> peer);
[[nodiscard]] rpl::producer<> HiddenChatsChanges(
	gsl::not_null<Main::Session*> session);
[[nodiscard]] bool AccountHidden(gsl::not_null<Main::Account*> account);
[[nodiscard]] gsl::not_null<Main::Account*> VisibleAccount(
	gsl::not_null<Main::Account*> account);

} // namespace Serein::Privacy
