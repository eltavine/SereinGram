#pragma once

#include "base/basic_types.h"
#include "ui/text/text_entity.h"

#include <gsl/pointers>
#include <rpl/rpl.h>

class PeerData;

namespace Serein::Privacy {

using ProfileRow = Fn<void(
	rpl::producer<QString> label,
	rpl::producer<TextWithEntities> value)>;

void FillProfileRows(not_null<PeerData*> peer, const ProfileRow &add);

} // namespace Serein::Privacy
