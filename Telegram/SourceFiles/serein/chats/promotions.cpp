#include "serein/hooks/chats/promotions.h"

#include "serein/chats/options.h"
#include "dialogs/suggestions/suggestion.h"

namespace Serein::Chats {

bool HideSponsoredMessages() {
	return ForDevice().Get(kHideSponsoredMessages);
}

bool HideProxySponsor() {
	return ForDevice().Get(kHideProxySponsor);
}

bool HideSuggestion(Dialogs::TopBarSuggestions::Priority priority) {
	using Priority = Dialogs::TopBarSuggestions::Priority;
	switch (priority) {
	case Priority::BirthdaySetup:
	case Priority::BirthdayContacts:
		return ForDevice().Get(kHideBirthdaySuggestions);
	case Priority::PremiumOffer:
	case Priority::PremiumGrace:
	case Priority::LowCreditsSubs:
	case Priority::CustomPromo:
	case Priority::GiftAuctions:
		return ForDevice().Get(kHidePremiumPromotions);
	default:
		return false;
	}
}

} // namespace Serein::Chats
