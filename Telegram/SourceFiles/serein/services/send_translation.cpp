#include "serein/services/send_translation.h"
#include "serein/hooks/compose/confirm.h"

#include "serein/core/options.h"
#include "serein/features/send_translation/model/languages.h"
#include "serein/schema/gen/settings/services.h"
#include "serein/services/draft_translation.h"
#include "boxes/translate_box.h"
#include "data/data_peer.h"
#include "data/data_peer_id.h"
#include "data/data_session.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "main/session/session_show.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/fields/input_field.h"
#include "ui/widgets/labels.h"

#include "styles/style_layers.h"

namespace Serein {
namespace {

[[nodiscard]] ServicesSchema::SendTranslations Read(
		not_null<Main::Session*> session) {
	return ServicesSchema::ReadSendTranslations(
		ForAccount(session).Get(ServiceSettings::kSendTranslations));
}

[[nodiscard]] LanguageId Language(not_null<PeerData*> peer) {
	const auto name = ServicesSchema::SendLanguage(
		Read(&peer->session()),
		SerializePeerId(peer->id));
	return name.isEmpty() ? LanguageId() : LanguageId::FromName(name);
}

void Save(
		const std::shared_ptr<Main::SessionShow> &show,
		not_null<PeerData*> peer,
		LanguageId language) {
	const auto session = &peer->session();
	const auto updated = ServicesSchema::WithSendLanguage(
		Read(session),
		SerializePeerId(peer->id),
		language ? language.name() : QString());
	if (!updated) {
		show->showToast(tr::lng_serein_send_translation_full(tr::now));
		return;
	}
	const auto raw = updated->languages.empty()
		? QByteArray()
		: ServicesSchema::SerializeSendTranslations(*updated);
	Expects(ForAccount(session).Set(ServiceSettings::kSendTranslations, raw));
}

void ChooseLanguage(
		std::shared_ptr<Main::SessionShow> show,
		not_null<PeerData*> peer) {
	const auto current = Language(peer);
	show->showBox(Ui::ChooseTranslateToBox(
		current ? current : Ui::ChooseTranslateTo(LanguageId()),
		[=](LanguageId chosen) { Save(show, peer, chosen); }));
}

[[nodiscard]] std::optional<TextWithTags> &Approved(
		not_null<Ui::InputField*> field) {
	static auto approved = base::flat_map<
		not_null<Ui::InputField*>,
		std::optional<TextWithTags>>();
	auto i = approved.find(field);
	if (i == end(approved)) {
		i = approved.emplace(field, std::nullopt).first;
		field->lifetime().add([=] { approved.remove(field); });
	}
	return i->second;
}

} // namespace

QString SendTranslationName(not_null<PeerData*> peer) {
	const auto language = Language(peer);
	return language ? language.locale().nativeLanguageName() : QString();
}

std::vector<not_null<PeerData*>> SendTranslationPeers(
		not_null<Main::Session*> session) {
	auto result = std::vector<not_null<PeerData*>>();
	for (const auto &[key, language] : Read(session).languages) {
		auto ok = false;
		const auto id = DeserializePeerId(key.toULongLong(&ok));
		if (ok && id && QString::number(SerializePeerId(id)) == key) {
			result.push_back(session->data().peer(id));
		}
	}
	return result;
}

void ChooseSendTranslation(
		std::shared_ptr<Main::SessionShow> show,
		not_null<PeerData*> peer) {
	if (!Language(peer)) {
		ChooseLanguage(show, peer);
		return;
	}
	show->showBox(Box([=](not_null<Ui::GenericBox*> box) {
		box->setTitle(tr::lng_serein_send_translation_language());
		box->addRow(object_ptr<Ui::FlatLabel>(
			box,
			rpl::single(SendTranslationName(peer)),
			st::boxLabel));
		box->addButton(tr::lng_serein_send_translation_change(), [=] {
			box->closeBox();
			ChooseLanguage(show, peer);
		});
		box->addButton(tr::lng_serein_send_translation_off(), [=] {
			Save(show, peer, LanguageId());
			box->closeBox();
		});
		box->addButton(tr::lng_cancel(), [=] { box->closeBox(); });
	}));
}

} // namespace Serein

namespace Serein::Compose {

bool TranslateBeforeSend(
		std::shared_ptr<Main::SessionShow> show,
		not_null<PeerData*> peer,
		Ui::InputField *field,
		Fn<void()> resend) {
	if (!field || field->empty()) {
		return false;
	}
	const auto language = Language(peer);
	if (!language) {
		return false;
	}
	auto &approved = Approved(field);
	if (base::take(approved) == field->getTextWithTags()) {
		return false;
	}
	ShowTranslationBox(show, field, language, [=](
			not_null<Ui::GenericBox*> box,
			TextWithTags original,
			Fn<std::optional<TextWithEntities>()> result) {
		box->setTitle(tr::lng_serein_send_translation());
		const auto send = [=](Fn<void()> apply) {
			if (field->getTextWithTags() != original) {
				box->showToast(tr::lng_serein_draft_changed(tr::now));
				return;
			}
			apply();
			Approved(field) = field->getTextWithTags();
			box->closeBox();
			resend();
		};
		box->addButton(tr::lng_serein_send_translation_send(), crl::guard(
			field,
			[=] {
				if (const auto translation = result()) {
					send([=] { ApplyTranslation(field, *translation); });
				}
			}));
		box->addButton(
			tr::lng_serein_send_translation_original(),
			crl::guard(field, [=] { send([] {}); }));
		box->addButton(tr::lng_cancel(), [=] { box->closeBox(); });
	});
	return true;
}

} // namespace Serein::Compose
