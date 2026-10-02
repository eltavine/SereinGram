#include "serein/features/stories/editor.h"

#include "serein/features/stories/audience.h"
#include "serein/features/stories/common.h"
#include "api/api_text_entities.h"
#include "apiwrap.h"
#include "chat_helpers/compose/compose_show.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "mtproto/mtproto_response.h"
#include "ui/layers/generic_box.h"
#include "ui/text/text_entity.h"
#include "ui/widgets/fields/input_field.h"
#include "styles/style_layers.h"

namespace Serein::Stories {
namespace {

struct Loaded {
	TextWithEntities caption;
	std::optional<AudienceValue> audience;
};

[[nodiscard]] std::vector<std::uint64_t> Ids(
		const MTPVector<MTPlong> &users) {
	auto result = std::vector<std::uint64_t>();
	result.reserve(users.v.size());
	for (const auto &id : users.v) {
		result.push_back(id.v);
	}
	return result;
}

[[nodiscard]] bool AppendRule(
		std::vector<AudienceRule> &rules,
		const MTPPrivacyRule &rule) {
	using Kind = AudienceRule::Kind;
	return rule.match([&](const MTPDprivacyValueAllowAll &) {
		rules.push_back({ Kind::AllowAll });
		return true;
	}, [&](const MTPDprivacyValueAllowContacts &) {
		rules.push_back({ Kind::AllowContacts });
		return true;
	}, [&](const MTPDprivacyValueAllowCloseFriends &) {
		rules.push_back({ Kind::AllowCloseFriends });
		return true;
	}, [&](const MTPDprivacyValueAllowUsers &data) {
		rules.push_back({ Kind::AllowUsers, Ids(data.vusers()) });
		return true;
	}, [&](const MTPDprivacyValueDisallowUsers &data) {
		rules.push_back({ Kind::DisallowUsers, Ids(data.vusers()) });
		return true;
	}, [&](const MTPDprivacyValueDisallowAll &) {
		return true;
	}, [](const auto &) {
		return false;
	});
}

[[nodiscard]] std::optional<AudienceValue> ReadAudience(
		not_null<Main::Session*> session,
		const MTPVector<MTPPrivacyRule> &privacy) {
	auto rules = std::vector<AudienceRule>();
	for (const auto &rule : privacy.v) {
		if (!AppendRule(rules, rule)) {
			return std::nullopt;
		}
	}
	return AudienceValueFor(session, ParseAudience(rules));
}

[[nodiscard]] std::optional<Loaded> ReadStory(
		not_null<PeerData*> peer,
		const MTPstories_Stories &result,
		StoryId id) {
	const auto session = &peer->session();
	const auto &data = result.data();
	session->data().processUsers(data.vusers());
	session->data().processChats(data.vchats());
	for (const auto &item : data.vstories().v) {
		if (item.type() != mtpc_storyItem) {
			continue;
		}
		const auto &story = item.c_storyItem();
		if (story.vid().v != id) {
			continue;
		}
		auto loaded = Loaded{
			.caption = {
				story.vcaption().value_or_empty(),
				Api::EntitiesFromMTP(
					session,
					story.ventities().value_or_empty()),
			},
		};
		if (const auto privacy = story.vprivacy()
			; privacy && peer->isSelf()) {
			loaded.audience = ReadAudience(session, *privacy);
		}
		return loaded;
	}
	return std::nullopt;
}

void EditBox(
		not_null<Ui::GenericBox*> box,
		std::shared_ptr<ChatHelpers::Show> show,
		not_null<PeerData*> peer,
		StoryId id,
		Loaded loaded) {
	box->setTitle(tr::lng_serein_story_edit());
	box->setWidth(st::boxWideWidth);
	const auto caption = AddCaptionField(box, show);
	caption->setTextWithTags({
		loaded.caption.text,
		TextUtilities::ConvertEntitiesToTextTags(loaded.caption.entities),
	}, Ui::InputField::HistoryAction::Clear);
	const auto audience = loaded.audience.has_value();
	const auto value = box->lifetime().make_state<
		rpl::variable<AudienceValue>>(
			loaded.audience.value_or(AudienceValue()));
	if (audience) {
		AddAudienceSection(box->verticalLayout(), show, value);
	}
	const auto saving = box->lifetime().make_state<bool>(false);
	const auto weak = QPointer<Ui::GenericBox>(box.get());
	box->addButton(tr::lng_settings_save(), [=] {
		if (*saving) {
			return;
		}
		const auto rules = audience
			? AudienceRulesFor(value->current())
			: std::vector<AudienceRule>();
		if (audience && rules.empty()) {
			box->showToast(tr::lng_serein_story_need_people(tr::now));
			return;
		}
		*saving = true;
		const auto session = &show->session();
		const auto text = PrepareCaption(
			caption->getTextWithAppliedMarkdown());
		using Flag = MTPstories_EditStory::Flag;
		session->api().request(MTPstories_EditStory(
			MTP_flags(Flag::f_caption
				| (audience ? Flag::f_privacy_rules : Flag())),
			peer->input(),
			MTP_int(id),
			MTPInputMedia(),
			MTPVector<MTPMediaArea>(),
			MTP_string(text.text),
			Api::EntitiesToMTP(
				session,
				text.entities,
				Api::ConvertOption::SkipLocal),
			PrivacyRules(session, rules),
			MTPInputDocument()
		)).done([=](const MTPUpdates &result) {
			session->api().applyUpdates(result);
			show->showToast(tr::lng_serein_story_edited(tr::now));
			if (weak) {
				weak->closeBox();
			}
		}).fail([=](const MTP::Error &error) {
			if (weak) {
				*saving = false;
			}
			MTP::ShowErrorFallback(show, error);
		}).send();
	});
	box->addButton(tr::lng_cancel(), [=] { box->closeBox(); });
}

} // namespace

void EditStory(
		std::shared_ptr<ChatHelpers::Show> show,
		not_null<PeerData*> peer,
		StoryId id) {
	peer->session().api().request(MTPstories_GetStoriesByID(
		peer->input(),
		MTP_vector<MTPint>(1, MTP_int(id))
	)).done([=](const MTPstories_Stories &result) {
		if (auto loaded = ReadStory(peer, result, id)) {
			show->showBox(Box(EditBox, show, peer, id, std::move(*loaded)));
		}
	}).fail([=](const MTP::Error &error) {
		MTP::ShowErrorFallback(show, error);
	}).send();
}

} // namespace Serein::Stories
