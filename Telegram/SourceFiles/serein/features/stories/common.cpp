#include "serein/features/stories/common.h"

#include "chat_helpers/compose/compose_show.h"
#include "chat_helpers/message_field.h"
#include "data/data_session.h"
#include "data/data_user.h"
#include "lang/lang_keys.h"
#include "main/main_app_config.h"
#include "main/main_session.h"
#include "ui/layers/generic_box.h"
#include "ui/text/text_entity.h"
#include "ui/widgets/fields/input_field.h"
#include "styles/style_serein.h"

namespace Serein::Stories {
namespace {

constexpr auto kCaptionLimit = 200;
constexpr auto kPremiumCaptionLimit = 2048;

[[nodiscard]] int CaptionLimit(not_null<Main::Session*> session) {
	const auto &config = session->appConfig();
	return session->premium()
		? config.get<int>(
			u"story_caption_length_limit_premium"_q,
			kPremiumCaptionLimit)
		: config.get<int>(
			u"story_caption_length_limit_default"_q,
			kCaptionLimit);
}

[[nodiscard]] MTPInputPrivacyRule PrivacyRule(
		not_null<Main::Session*> session,
		const AudienceRule &rule) {
	const auto users = [&] {
		auto result = QVector<MTPInputUser>();
		result.reserve(int(rule.users.size()));
		for (const auto id : rule.users) {
			result.push_back(session->data().user(UserId(id))->inputUser());
		}
		return MTP_vector<MTPInputUser>(std::move(result));
	};
	using Kind = AudienceRule::Kind;
	switch (rule.kind) {
	case Kind::AllowAll: return MTP_inputPrivacyValueAllowAll();
	case Kind::AllowContacts: return MTP_inputPrivacyValueAllowContacts();
	case Kind::AllowCloseFriends:
		return MTP_inputPrivacyValueAllowCloseFriends();
	case Kind::AllowUsers: return MTP_inputPrivacyValueAllowUsers(users());
	case Kind::DisallowUsers:
		return MTP_inputPrivacyValueDisallowUsers(users());
	}
	Unexpected("Rule kind in Serein::Stories::PrivacyRule.");
}

} // namespace

QString ErrorText(const QString &type) {
	switch (ClassifyError(type)) {
	case PostError::TooMany:
		return tr::lng_serein_story_error_limit(tr::now);
	case PostError::PremiumRequired:
		return tr::lng_serein_story_error_premium(tr::now);
	case PostError::BoostsRequired:
		return tr::lng_serein_story_error_boosts(tr::now);
	case PostError::WeeklyLimit:
		return tr::lng_serein_story_error_weekly(tr::now);
	case PostError::MonthlyLimit:
		return tr::lng_serein_story_error_monthly(tr::now);
	case PostError::Other:
		break;
	}
	return type.isEmpty() ? tr::lng_attach_failed(tr::now) : type;
}

TextWithEntities PrepareCaption(const TextWithTags &caption) {
	auto result = TextWithEntities{
		caption.text,
		TextUtilities::ConvertTextTagsToEntities(caption.tags),
	};
	TextUtilities::PrepareForSending(
		result,
		TextParseLinks | TextParseMentions | TextParseHashtags);
	TextUtilities::Trim(result);
	return result;
}

MTPVector<MTPInputPrivacyRule> PrivacyRules(
		not_null<Main::Session*> session,
		const std::vector<AudienceRule> &rules) {
	auto result = QVector<MTPInputPrivacyRule>();
	result.reserve(int(rules.size()));
	for (const auto &rule : rules) {
		result.push_back(PrivacyRule(session, rule));
	}
	return MTP_vector<MTPInputPrivacyRule>(std::move(result));
}

not_null<Ui::InputField*> AddCaptionField(
		not_null<Ui::GenericBox*> box,
		std::shared_ptr<ChatHelpers::Show> show) {
	const auto session = &show->session();
	const auto field = box->addRow(object_ptr<Ui::InputField>(
		box,
		st::sereinStoryCaption,
		Ui::InputField::Mode::MultiLine,
		tr::lng_photo_caption()));
	InitMessageFieldHandlers({
		.session = session,
		.show = show,
		.field = field,
		.customEmojiPaused = [=] {
			return show->paused(ChatHelpers::PauseReason::Layer);
		},
	});
	field->setMaxLength(CaptionLimit(session));
	box->setFocusCallback([=] { field->setFocusFast(); });
	return field;
}

} // namespace Serein::Stories
