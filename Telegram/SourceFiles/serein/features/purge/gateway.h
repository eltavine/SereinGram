#pragma once

#include "serein/features/purge/model/job.h"

#include <gsl/pointers>

class PeerData;

namespace Serein::Purge {

using Identities = std::vector<gsl::not_null<PeerData*>>;

[[nodiscard]] Identities IdentitiesFor(gsl::not_null<PeerData*> chat);
[[nodiscard]] std::shared_ptr<Gateway> MakeGateway(
	gsl::not_null<PeerData*> chat,
	Identities identities);
void CountMessages(
	gsl::not_null<PeerData*> chat,
	gsl::not_null<PeerData*> identity,
	qint64 before,
	Fn<void(int)> done);

} // namespace Serein::Purge
