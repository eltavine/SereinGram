#include "serein/hooks/compose/text.h"

#include "serein/compose/options.h"
#include "serein/compose/spacing.h"
#include "serein/display/text_entities.h"
#include "ui/text/text_utilities.h"
#include "ui/widgets/fields/input_field.h"
#include "ui/widgets/popup_menu.h"
#include "lang/lang_keys.h"

#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include <algorithm>
#include <vector>

namespace Serein::Compose {

TextWithEntities AddChineseLatinSpacing(const TextWithEntities &text) {
	const auto length = int(text.text.size());
	if (length < 2) {
		return text;
	}
	for (const auto &entity : text.entities) {
		if (!entity.validForText(length)) {
			return text;
		}
	}
	auto boundaries = std::vector<int>(length + 1);
	const auto protect = [&](const EntitiesInText &entities) {
		for (const auto &entity : entities) {
			if (Display::VerbatimEntity(entity.type())
				&& entity.validForText(length)) {
				++boundaries[entity.offset() + 1];
				--boundaries[entity.offset() + entity.length()];
			}
		}
	};
	protect(text.entities);
	protect(TextUtilities::ParseEntities(text.text,
		TextParseLinks | TextParseMentions | TextParseHashtags | TextParseBotCommands
	).entities);
	auto codeStart = -1;
	auto codeTicks = 0;
	for (auto i = 0; i < length;) {
		if (text.text.at(i) == u'\\' && codeStart < 0) {
			i += std::min(2, length - i);
		} else if (text.text.at(i) != u'`') {
			++i;
		} else {
			const auto start = i;
			while (i < length && text.text.at(i) == u'`') {
				++i;
			}
			if (codeStart < 0) {
				codeStart = start;
				codeTicks = i - start;
			} else if (i - start == codeTicks) {
				++boundaries[codeStart + 1];
				--boundaries[i];
				codeStart = -1;
			}
		}
	}
	if (codeStart >= 0) {
		++boundaries[codeStart + 1];
		--boundaries[length];
	}

	const auto spaced = InsertChineseLatinSpacing(text.text, boundaries);
	if (spaced.text == text.text) {
		return text;
	}
	auto result = TextWithEntities();
	result.text = spaced.text;
	for (const auto &entity : text.entities) {
		const auto start = spaced.after[entity.offset()];
		const auto end = spaced.before[entity.offset() + entity.length()];
		result.entities.push_back(EntityInText(
			entity.type(), start, end - start, entity.data()));
	}
	return result;
}

TextWithEntities PrepareText(const TextWithEntities &text, bool editing) {
	const auto enabled = ForDevice().Get(editing ? kSpaceOnEdit : kSpaceOnSend);
	auto result = enabled ? AddChineseLatinSpacing(text) : text;
	const auto language = ForDevice().Get(kDefaultCodeLanguage);
	if (!language.isEmpty()) {
		for (auto &entity : result.entities) {
			if (entity.type() == EntityType::Pre && entity.data().isEmpty()) {
				entity = EntityInText(entity.type(), entity.offset(),
					entity.length(), language);
			}
		}
	}
	return result;
}

QStringList QuickReplies() {
	const auto raw = ForDevice().Get(kQuickReplies);
	const auto document = QJsonDocument::fromJson(raw);
	if (!document.isObject()) {
		return {};
	}
	const auto values = document.object().value(u"replies"_q).toArray();
	if (values.size() != 2 || !values[0].isString() || !values[1].isString()) {
		return {};
	}
	return { values[0].toString(), values[1].toString() };
}

bool SetQuickReplies(const QStringList &replies) {
	if (replies.size() != 2) {
		return false;
	}
	const auto raw = QJsonDocument(QJsonObject{
		{ u"version"_q, 1 },
		{ u"replies"_q, QJsonArray{ replies[0], replies[1] } },
	}).toJson(QJsonDocument::Compact);
	return ForDevice().Set(kQuickReplies, raw);
}

void InstallQuickReplies(not_null<Ui::InputField*> field) {
	field->addContextMenuHook([=](Ui::InputField::ContextMenuRequest request) {
		const auto replies = QuickReplies();
		for (auto i = 0; i != replies.size(); ++i) {
			const auto reply = replies[i];
			if (reply.isEmpty()) {
				continue;
			}
			request.menu->addAction(tr::lng_serein_quick_reply_insert(
				tr::now, lt_index, QString::number(i + 1)), field, [=] {
				auto cursor = field->textCursor();
				cursor.beginEditBlock();
				cursor.insertText(reply);
				cursor.endEditBlock();
				field->setTextCursor(cursor);
			});
		}
	});
}

} // namespace Serein::Compose
