#include "serein/compose/mention.h"

#include "serein/compose/mention_query.h"
#include "serein/compose/options.h"
#include "apiwrap.h"
#include "chat_helpers/message_field.h"
#include "data/data_peer_id.h"
#include "data/data_session.h"
#include "data/data_user.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "main/session/session_show.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/fields/input_field.h"
#include "ui/widgets/popup_menu.h"
#include "styles/style_layers.h"
#include "styles/style_widgets.h"

#include <QtCore/QPointer>
#include <QtGui/QTextCursor>
#include <QtGui/QTextDocument>

namespace Serein::Compose {
namespace {

struct Selection {
	int from = 0;
	int till = 0;
	QString text;
};

[[nodiscard]] std::optional<Selection> MentionSelection(
		not_null<Ui::InputField*> field) {
	const auto cursor = field->textCursor();
	if (!cursor.hasSelection()) {
		return std::nullopt;
	}
	const auto text = cursor.selectedText();
	if (text.trimmed().isEmpty()
		|| text.contains(QChar::ObjectReplacementCharacter)
		|| text.contains(QChar::ParagraphSeparator)
		|| text.contains(QChar::LineSeparator)) {
		return std::nullopt;
	}
	return Selection{
		.from = cursor.selectionStart(),
		.till = cursor.selectionEnd(),
		.text = text,
	};
}

void ApplyMention(
		not_null<Ui::InputField*> field,
		const Selection &selection,
		not_null<UserData*> user) {
	auto cursor = field->textCursor();
	cursor.setPosition(selection.from);
	cursor.setPosition(selection.till, QTextCursor::KeepAnchor);
	if (cursor.selectedText() != selection.text) {
		return;
	}
	if (cursor.document()->characterAt(selection.till) == QChar(' ')) {
		cursor.setPosition(selection.till + 1, QTextCursor::KeepAnchor);
	}
	field->setTextCursor(cursor);
	field->insertTag(selection.text, PrepareMentionTag(user));
}

void ResolveUser(
		not_null<Main::Session*> session,
		const MentionQuery &query,
		Fn<void(UserData*)> done) {
	if (query.userId) {
		done(session->data().userLoaded(UserId(query.userId)));
		return;
	} else if (const auto peer = session->data().peerByUsername(
			query.username)) {
		done(peer->asUser());
		return;
	}
	session->api().request(MTPcontacts_ResolveUsername(
		MTP_flags(0),
		MTP_string(query.username),
		MTP_string(QString())
	)).done([=](const MTPcontacts_ResolvedPeer &result) {
		const auto &data = result.data();
		session->data().processUsers(data.vusers());
		session->data().processChats(data.vchats());
		const auto peer = session->data().peerLoaded(
			peerFromMTP(data.vpeer()));
		done(peer ? peer->asUser() : nullptr);
	}).fail([=] {
		done(nullptr);
	}).send();
}

void AskMention(
		std::shared_ptr<Main::SessionShow> show,
		not_null<Ui::InputField*> field,
		Selection selection) {
	const auto weak = QPointer<Ui::InputField>(field.get());
	show->showBox(Box([=](not_null<Ui::GenericBox*> box) {
		box->setTitle(tr::lng_serein_mention_title());
		const auto input = box->addRow(
			object_ptr<Ui::InputField>(
				box,
				st::defaultInputField,
				tr::lng_serein_mention_placeholder()),
			st::boxRowPadding);
		box->setFocusCallback([=] { input->setFocusFast(); });
		const auto submit = [=] {
			const auto query = ParseMentionQuery(input->getLastText());
			if (!query.userId && query.username.isEmpty()) {
				input->showError();
				return;
			}
			ResolveUser(&show->session(), query, crl::guard(box, [=](
					UserData *user) {
				if (!user) {
					input->showError();
					show->showToast(tr::lng_serein_mention_not_found(tr::now));
					return;
				}
				if (const auto strong = weak.data()) {
					ApplyMention(strong, selection, user);
				}
				box->closeBox();
			}));
		};
		input->submits() | rpl::on_next(submit, input->lifetime());
		box->addButton(tr::lng_box_done(), submit);
		box->addButton(tr::lng_cancel(), [=] { box->closeBox(); });
	}));
}

} // namespace

void InstallMention(
		not_null<Ui::InputField*> field,
		std::shared_ptr<Main::SessionShow> show) {
	const auto weak = std::weak_ptr<Main::SessionShow>(show);
	field->addContextMenuHook([=](Ui::InputField::ContextMenuRequest request) {
		if (!ForDevice().Get(kMentionMenu)) {
			return;
		}
		const auto selection = MentionSelection(field);
		if (!selection) {
			return;
		}
		request.menu->addAction(
			tr::lng_serein_mention_create(tr::now),
			field,
			crl::guard(field, [=] {
				if (const auto locked = weak.lock()) {
					AskMention(locked, field, *selection);
				}
			}));
	});
}

} // namespace Serein::Compose
