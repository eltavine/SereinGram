#include "serein/settings/ayu_migration.h"

#include "base/basic_types.h"
#include "serein/schema/gen/settings/chats.h"
#include "serein/schema/gen/settings/compose.h"
#include "serein/schema/gen/settings/ghost.h"
#include "serein/schema/gen/settings/history.h"
#include "serein/schema/gen/settings/interface.h"
#include "serein/schema/gen/settings/media.h"
#include "serein/schema/gen/settings/messages.h"
#include "serein/schema/gen/settings/privacy.h"
#include "serein/schema/gen/settings/services.h"

#include <QtCore/QJsonDocument>

#include <algorithm>

namespace Serein {
namespace {

struct Switch {
	const char *key;
	const Option<bool> *option;
	bool invert = false;
};

constexpr Switch kSwitches[] = {
	{ "saveDeletedMessages", &HistorySettings::kHistorySaveDeleted },
	{ "saveMessagesHistory", &HistorySettings::kHistorySaveEdits },
	{ "saveForBots", &HistorySettings::kHistoryIncludeBots },
	{ "semiTransparentDeletedMessages", &Messages::kFadeDeletedMessages },
	{ "disableAds", &Chats::kHideSponsoredMessages },
	{ "disableStories", &Chats::kHideStories },
	{ "hideSimilarChannels", &Messages::kHideRecommendedChannels },
	{ "removeMessageTail", &Interface::kHideBubbleTail },
	{ "spoofWebviewAsAndroid", &ServiceSettings::kAndroidWebApps },
	{ "showChannelReactions", &Messages::kHideChannelReactions, true },
	{ "showGroupReactions", &Messages::kHideGroupReactions, true },
	{ "showPrivateChatReactions", &Messages::kHidePrivateReactions, true },
	{ "hideFastShare", &Messages::kHideQuickShare },
	{ "showReactionsPanelInContextMenu", &Messages::kHideReactionMenu, true },
	{ "showAttachButtonInMessageField", &Compose::kHideAttachButton, true },
	{ "showCommandsButtonInMessageField", &Compose::kHideBotCommandButton, true },
	{ "showEmojiButtonInMessageField", &Compose::kHideEmojiButton, true },
	{ "showMicrophoneButtonInMessageField", &Compose::kHideRecordingButton, true },
	{ "showAutoDeleteButtonInMessageField", &Compose::kHideAutoDeleteButton, true },
	{ "showGiftButtonInMessageField", &Compose::kHideGiftButton, true },
	{ "showAiEditorButtonInMessageField", &Compose::kHideAiButton, true },
	{ "showAttachPopup", &Compose::kDisableAttachHover, true },
	{ "showEmojiPopup", &Compose::kDisableEmojiHover, true },
	{ "hideNotificationBadge", &Interface::kHideAppIconBadge },
	{ "hideAllChatsFolder", &Chats::kHideAllChatsFolder },
	{ "quickAdminShortcuts", &Chats::kManagementShortcuts },
	{ "disableGreetingSticker", &Media::kHideGreetingSticker },
	{ "showMessageSeconds", &Messages::kSecondsInMessages },
	{ "stickerConfirmation", &Compose::kConfirmSticker },
	{ "gifConfirmation", &Compose::kConfirmGif },
	{ "voiceConfirmation", &Compose::kPreviewVoice },
	{ "roundConfirmation", &Compose::kPreviewRoundVideo },
};

constexpr Switch kGhostSwitches[] = {
	{ "sendReadMessages", &Ghost::kGhostHideReadReceipts, true },
	{ "sendReadStories", &Ghost::kGhostHideStoryViews, true },
	{ "sendOnlinePackets", &Ghost::kGhostHideOnline, true },
	{ "sendUploadProgress", &Ghost::kGhostHideTyping, true },
};

struct OptionsJson {
	QJsonObject device;
	QJsonObject account;

	template <typename Type>
	void set(const Option<Type> &option, const Type &value) {
		auto &target = (option.scope == Scope::Account) ? account : device;
		target.insert(
			QString::fromUtf8(option.key.data(), option.key.size()),
			QJsonValue(value));
	}
};

void AppendSkipped(QStringList &skipped, const QJsonObject &values) {
	for (auto i = values.begin(); i != values.end(); ++i) {
		skipped.push_back(i.key().left(120).simplified());
	}
}

[[nodiscard]] bool MigrateGhost(
		QJsonObject &source,
		quint64 userId,
		OptionsJson &options,
		QStringList &skipped) {
	const auto profiles = source.take(u"ghostModeSettings"_q);
	const auto global = source.take(u"useGlobalGhostMode"_q);
	if ((!global.isUndefined() && !global.isBool())
		|| (!profiles.isUndefined() && !profiles.isObject())) {
		return false;
	}

	auto selected = QJsonObject();
	if (profiles.isObject()) {
		const auto profile = profiles.toObject().value(
			global.toBool(true) ? u"0"_q : QString::number(userId));
		if (profile.isUndefined()) {
			if (!profiles.toObject().isEmpty()) {
				skipped.push_back(u"ghostModeSettings"_q);
			}
			return true;
		} else if (!profile.isObject()) {
			return false;
		}
		selected = profile.toObject();
	} else if (std::ranges::none_of(kGhostSwitches, [&](const Switch &entry) {
		return source.contains(QLatin1String(entry.key));
	})) {
		return true;
	}
	auto &values = profiles.isObject() ? selected : source;
	auto enabled = false;
	for (const auto &entry : kGhostSwitches) {
		const auto value = values.take(QLatin1String(entry.key));
		if (!value.isUndefined() && !value.isBool()) {
			return false;
		}
		const auto hide = !value.toBool(true);
		enabled = enabled || hide;
		options.set(*entry.option, hide);
	}
	options.set(Ghost::kGhostMode, enabled);
	if (profiles.isObject()) {
		AppendSkipped(skipped, selected);
	}
	return true;
}

} // namespace

AyuMigration MigrateAyuSettings(const QJsonObject &source, quint64 userId) {
	auto remaining = source;
	auto options = OptionsJson();
	auto result = AyuMigration();
	if (!MigrateGhost(remaining, userId, options, result.skippedKeys)) {
		return {};
	}
	for (const auto &entry : kSwitches) {
		const auto value = remaining.take(QLatin1String(entry.key));
		if (value.isUndefined()) {
			continue;
		} else if (!value.isBool()) {
			return {};
		}
		options.set(*entry.option, value.toBool() != entry.invert);
	}
	const auto font = remaining.take(u"monoFont"_q);
	if (!font.isUndefined()) {
		const auto family = font.toString();
		if (!font.isString() || family.size() > 256
			|| std::ranges::any_of(family, [](QChar ch) {
				return ch.category() == QChar::Other_Control;
			})) {
			return {};
		}
		options.set(Interface::kMonospaceFont, family);
	}
	const auto peerId = remaining.take(u"showPeerId"_q);
	if (!peerId.isUndefined()) {
		switch (peerId.toInt(-1)) {
		case 0: options.set(Privacy::kProfileIdFormat, 0); break;
		case 1: options.set(Privacy::kProfileIdFormat, 2); break;
		case 2: options.set(Privacy::kProfileIdFormat, 1); break;
		default: return {};
		}
	}
	AppendSkipped(result.skippedKeys, remaining);
	if (!options.device.isEmpty() || !options.account.isEmpty()) {
		result.settings = QJsonDocument(QJsonObject{
			{ u"version"_q, 2 },
			{ u"options"_q, options.device },
			{ u"account"_q, options.account },
		}).toJson(QJsonDocument::Compact);
	}
	return result;
}

} // namespace Serein
