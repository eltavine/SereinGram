#include "serein/adapters/tdesktop/native_settings.h"

#include "core/application.h"
#include "core/core_settings.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "main/main_session_settings.h"

#include <array>

namespace Serein::Adapters {
namespace {

template <typename Settings>
struct BooleanSetting {
	const char *key;
	tr::phrase<> title;
	bool (Settings::*read)() const;
	void (Settings::*write)(bool);
};

[[nodiscard]] const auto &DeviceSettings() {
	static const auto result = std::array{
		BooleanSetting<Core::Settings>{ "soundNotify", tr::lng_serein_native_sound, &Core::Settings::soundNotify, &Core::Settings::setSoundNotify },
		BooleanSetting<Core::Settings>{ "desktopNotify", tr::lng_serein_native_desktop, &Core::Settings::desktopNotify, &Core::Settings::setDesktopNotify },
		BooleanSetting<Core::Settings>{ "largeEmoji", tr::lng_serein_native_large_emoji, &Core::Settings::largeEmoji, &Core::Settings::setLargeEmoji },
		BooleanSetting<Core::Settings>{ "replaceEmoji", tr::lng_serein_native_replace_emoji, &Core::Settings::replaceEmoji, &Core::Settings::setReplaceEmoji },
		BooleanSetting<Core::Settings>{ "suggestEmoji", tr::lng_serein_native_suggest_emoji, &Core::Settings::suggestEmoji, &Core::Settings::setSuggestEmoji },
		BooleanSetting<Core::Settings>{ "suggestStickersByEmoji", tr::lng_serein_native_suggest_stickers, &Core::Settings::suggestStickersByEmoji, &Core::Settings::setSuggestStickersByEmoji },
		BooleanSetting<Core::Settings>{ "loopAnimatedStickers", tr::lng_serein_native_loop_stickers, &Core::Settings::loopAnimatedStickers, &Core::Settings::setLoopAnimatedStickers },
		BooleanSetting<Core::Settings>{ "spellcheckerEnabled", tr::lng_serein_native_spellcheck, &Core::Settings::spellcheckerEnabled, &Core::Settings::setSpellcheckerEnabled },
		BooleanSetting<Core::Settings>{ "includeMutedCounter", tr::lng_serein_native_muted_counter, &Core::Settings::includeMutedCounter, &Core::Settings::setIncludeMutedCounter },
		BooleanSetting<Core::Settings>{ "countUnreadMessages", tr::lng_serein_native_count_messages, &Core::Settings::countUnreadMessages, &Core::Settings::setCountUnreadMessages },
		BooleanSetting<Core::Settings>{ "notifyAboutPinned", tr::lng_serein_native_pinned_notifications, &Core::Settings::notifyAboutPinned, &Core::Settings::setNotifyAboutPinned },
		BooleanSetting<Core::Settings>{ "hardwareAcceleratedVideo", tr::lng_serein_native_video_acceleration, &Core::Settings::hardwareAcceleratedVideo, &Core::Settings::setHardwareAcceleratedVideo },
	};
	return result;
}

[[nodiscard]] const auto &AccountSettings() {
	static const auto result = std::array{
		BooleanSetting<Main::SessionSettings>{ "archiveCollapsed", tr::lng_serein_native_archive_collapsed, &Main::SessionSettings::archiveCollapsed, &Main::SessionSettings::setArchiveCollapsed },
		BooleanSetting<Main::SessionSettings>{ "archiveInMainMenu", tr::lng_serein_native_archive_menu, &Main::SessionSettings::archiveInMainMenu, &Main::SessionSettings::setArchiveInMainMenu },
		BooleanSetting<Main::SessionSettings>{ "dialogsFiltersEnabled", tr::lng_serein_native_chat_folders, &Main::SessionSettings::dialogsFiltersEnabled, &Main::SessionSettings::setDialogsFiltersEnabled },
	};
	return result;
}

template <typename Settings, typename Entries>
void Export(QJsonObject &result, Settings &settings, const Entries &entries, const QString &prefix) {
	for (const auto &entry : entries) {
		result.insert(prefix + QString::fromLatin1(entry.key), (settings.*entry.read)());
	}
}

template <typename Settings, typename Entries>
void Apply(Settings &settings, const Entries &entries, const QJsonObject &values, const QString &prefix) {
	for (const auto &entry : entries) {
		const auto value = values.value(prefix + QString::fromLatin1(entry.key));
		if (!value.isUndefined()) {
			(settings.*entry.write)(value.toBool());
		}
	}
}

[[nodiscard]] QString Title(const QString &key) {
	for (const auto &entry : DeviceSettings()) {
		if (key == u"device."_q + QString::fromLatin1(entry.key)) return entry.title(tr::now);
	}
	for (const auto &entry : AccountSettings()) {
		if (key == u"account."_q + QString::fromLatin1(entry.key)) return entry.title(tr::now);
	}
	return QString();
}

} // namespace

QJsonObject ExportNativeSettings(not_null<Main::Session*> session) {
	auto result = QJsonObject();
	Export(result, Core::App().settings(), DeviceSettings(), u"device."_q);
	Export(result, session->settings(), AccountSettings(), u"account."_q);
	return result;
}

NativeSettingsPlan PlanNativeSettings(
		not_null<Main::Session*> session,
		const QJsonObject &values) {
	auto result = NativeSettingsPlan();
	const auto before = ExportNativeSettings(session);
	for (auto i = values.begin(); i != values.end(); ++i) {
		if (!before.contains(i.key()) || !i.value().isBool()) {
			result.error = tr::lng_serein_native_invalid(tr::now);
			return result;
		}
		if (before.value(i.key()) != i.value()) {
			result.preview.push_back(Title(i.key()) + u": "_q
				+ (before.value(i.key()).toBool() ? tr::lng_serein_config_on(tr::now) : tr::lng_serein_config_off(tr::now))
				+ u" → "_q
				+ (i.value().toBool() ? tr::lng_serein_config_on(tr::now) : tr::lng_serein_config_off(tr::now)));
		}
	}
	result.unchanged = [=] { return ExportNativeSettings(session) == before; };
	result.apply = [=] {
		Apply(Core::App().settings(), DeviceSettings(), values, u"device."_q);
		Apply(session->settings(), AccountSettings(), values, u"account."_q);
		Core::App().saveSettingsDelayed();
		session->saveSettingsDelayed();
	};
	return result;
}

} // namespace Serein::Adapters
