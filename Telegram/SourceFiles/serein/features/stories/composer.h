#pragma once

#include <gsl/pointers>

#include <memory>

class PeerData;

namespace ChatHelpers {
class Show;
} // namespace ChatHelpers

namespace Data {
class Story;
} // namespace Data

namespace Serein::Stories {

void StartPosting(
	std::shared_ptr<ChatHelpers::Show> show,
	gsl::not_null<PeerData*> peer);
void StartRepost(
	std::shared_ptr<ChatHelpers::Show> show,
	gsl::not_null<Data::Story*> story);

} // namespace Serein::Stories
