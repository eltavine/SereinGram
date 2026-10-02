#pragma once

#include <QtCore/QString>

namespace Serein::Compose {

[[nodiscard]] QString InlineBotForLink(const QString &text);

template <typename Query, typename Session>
[[nodiscard]] bool ApplyLinkInlineBot(
		Query &result,
		Session *session,
		const QString &text) {
	const auto username = InlineBotForLink(text);
	if (username.isEmpty()) {
		return false;
	}
	result.username = username;
	result.query = text.trimmed();
	result.bot = nullptr;
	result.lookingUpBot = true;
	if (const auto peer = session->data().peerByUsername(username)) {
		const auto user = peer->asUser();
		result.lookingUpBot = false;
		if (user
			&& user->isBot()
			&& !user->botInfo->inlinePlaceholder.isEmpty()) {
			result.bot = user;
		}
	}
	return true;
}

template <typename Field>
[[nodiscard]] bool LinkInlineBotDraft(const Field &field) {
	return !InlineBotForLink(field->getTextWithTags().text).isEmpty();
}

} // namespace Serein::Compose
