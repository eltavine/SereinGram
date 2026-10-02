#pragma once

#include "data/data_msg_id.h"

#include <gsl/pointers>

#include <memory>

class PeerData;

namespace ChatHelpers {
class Show;
} // namespace ChatHelpers

namespace Serein::Stories {

void EditStory(
	std::shared_ptr<ChatHelpers::Show> show,
	gsl::not_null<PeerData*> peer,
	StoryId id);

} // namespace Serein::Stories
