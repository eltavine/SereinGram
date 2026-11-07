#include "serein/adapters/tdesktop/native_settings.h"

#include "core/application.h"
#include "core/core_settings.h"
#include "data/data_session.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "main/main_session_settings.h"
#include "settings/settings_notifications_common.h"
#include "ui/text/format_values.h"
#include "ui/widgets/fields/input_field.h"
#include "window/notifications_manager.h"

#include <array>
#include <tuple>
#include <type_traits>
#include <vector>

namespace Serein::Adapters {
namespace {

template <typename Owner>
struct Setting {
	QString key;
	Fn<QString()> title;
	Fn<QJsonValue(const Owner&)> read;
	Fn<bool(const QJsonValue&)> accepts;
	Fn<void(Owner&, const QJsonValue&)> write;
	Fn<QString(const QJsonValue&)> format;
};

[[nodiscard]] QString ValueText(const QJsonValue &value) {
	if (value.isBool()) {
		return value.toBool()
			? tr::lng_serein_config_on(tr::now)
			: tr::lng_serein_config_off(tr::now);
	} else if (value.isString()) {
		return value.toString();
	}
	return QString::number(value.toDouble(), 'g', 15);
}

template <typename Value>
[[nodiscard]] QJsonValue Encode(Value value) {
	if constexpr (std::is_enum_v<Value>
		|| (std::is_integral_v<Value> && !std::is_same_v<Value, bool>)) {
		return QJsonValue(qint64(value));
	} else {
		return QJsonValue(value);
	}
}

template <typename Value>
[[nodiscard]] Value Decode(const QJsonValue &value) {
	if constexpr (std::is_same_v<Value, bool>) {
		return value.toBool();
	} else if constexpr (std::is_same_v<Value, QString>) {
		return value.toString();
	} else if constexpr (std::is_floating_point_v<Value>) {
		return Value(value.toDouble());
	} else {
		return Value(value.toInteger());
	}
}

template <typename Owner, typename Value, typename Argument>
[[nodiscard]] Setting<Owner> Field(
		QString key,
		tr::phrase<> title,
		Value (Owner::*read)() const,
		void (Owner::*write)(Argument),
		double maximum = 0.,
		double minimum = 0.) {
	return {
		.key = std::move(key),
		.title = [=] { return title(tr::now); },
		.read = [=](const Owner &owner) { return Encode((owner.*read)()); },
		.accepts = [=](const QJsonValue &value) {
			if constexpr (std::is_same_v<Value, bool>) {
				return value.isBool();
			} else if constexpr (std::is_same_v<Value, QString>) {
				if (!value.isString()) {
					return false;
				}
				const auto text = value.toString();
				return text.size() <= 256 && ranges::none_of(text, [](QChar ch) {
					return ch.category() == QChar::Other_Control;
				});
			} else {
				const auto number = std::is_floating_point_v<Value>
					? value.toDouble(-1.)
					: double(value.toInteger(-1));
				return number >= minimum && number <= maximum;
			}
		},
		.write = [=](Owner &owner, const QJsonValue &value) {
			(owner.*write)(Decode<Value>(value));
		},
		.format = ValueText,
	};
}

template <typename Owner, typename Value, size_t Size>
[[nodiscard]] Setting<Owner> Choice(
		QString key,
		tr::phrase<> title,
		Value (Owner::*read)() const,
		void (Owner::*write)(Value),
		std::array<tr::phrase<>, Size> choices) {
	auto result = Field(std::move(key), title, read, write, Size - 1);
	result.format = [=](const QJsonValue &value) {
		const auto index = value.toInt(-1);
		return (index >= 0 && size_t(index) < Size)
			? choices[index](tr::now)
			: ValueText(value);
	};
	return result;
}

[[nodiscard]] const auto &DeviceSettings() {
	using Settings = Core::Settings;
	static const auto result = std::array{
		Field(u"device.soundNotify"_q, tr::lng_serein_native_sound,
			&Settings::soundNotify, &Settings::setSoundNotify),
		Field(u"device.desktopNotify"_q, tr::lng_serein_native_desktop,
			&Settings::desktopNotify, &Settings::setDesktopNotify),
		Field(u"device.largeEmoji"_q, tr::lng_serein_native_large_emoji,
			&Settings::largeEmoji, &Settings::setLargeEmoji),
		Field(u"device.replaceEmoji"_q, tr::lng_serein_native_replace_emoji,
			&Settings::replaceEmoji, &Settings::setReplaceEmoji),
		Field(u"device.suggestEmoji"_q, tr::lng_serein_native_suggest_emoji,
			&Settings::suggestEmoji, &Settings::setSuggestEmoji),
		Field(u"device.suggestStickersByEmoji"_q, tr::lng_serein_native_suggest_stickers,
			&Settings::suggestStickersByEmoji, &Settings::setSuggestStickersByEmoji),
		Field(u"device.loopAnimatedStickers"_q, tr::lng_serein_native_loop_stickers,
			&Settings::loopAnimatedStickers, &Settings::setLoopAnimatedStickers),
		Field(u"device.spellcheckerEnabled"_q, tr::lng_serein_native_spellcheck,
			&Settings::spellcheckerEnabled, &Settings::setSpellcheckerEnabled),
		Field(u"device.includeMutedCounter"_q, tr::lng_serein_native_muted_counter,
			&Settings::includeMutedCounter, &Settings::setIncludeMutedCounter),
		Field(u"device.countUnreadMessages"_q, tr::lng_serein_native_count_messages,
			&Settings::countUnreadMessages, &Settings::setCountUnreadMessages),
		Field(u"device.notifyAboutPinned"_q, tr::lng_serein_native_pinned_notifications,
			&Settings::notifyAboutPinned, &Settings::setNotifyAboutPinned),
		Field(u"device.hardwareAcceleratedVideo"_q, tr::lng_serein_native_video_acceleration,
			&Settings::hardwareAcceleratedVideo, &Settings::setHardwareAcceleratedVideo),
		Field(u"device.adaptiveForWide"_q, tr::lng_settings_adaptive_wide,
			&Settings::adaptiveForWide, &Settings::setAdaptiveForWide),
		Field(u"device.askDownloadPath"_q, tr::lng_download_path_ask,
			&Settings::askDownloadPath, &Settings::setAskDownloadPath),
		Field(u"device.flashBounceNotify"_q, tr::lng_serein_native_attention,
			&Settings::flashBounceNotify, &Settings::setFlashBounceNotify),
		Field(u"device.skipToastsInFocus"_q, tr::lng_settings_skip_in_focus,
			&Settings::skipToastsInFocus, &Settings::setSkipToastsInFocus),
		Field(u"device.includeMutedCounterFolders"_q, tr::lng_settings_include_muted_folders,
			&Settings::includeMutedCounterFolders, &Settings::setIncludeMutedCounterFolders),
		Field(u"device.groupCallNoiseSuppression"_q, tr::lng_group_call_noise_suppression,
			&Settings::groupCallNoiseSuppression, &Settings::setGroupCallNoiseSuppression),
		Field(u"device.systemTextReplace"_q, tr::lng_settings_system_text_replace,
			&Settings::systemTextReplace, &Settings::setSystemTextReplace),
		Field(u"device.suggestAnimatedEmoji"_q, tr::lng_settings_suggest_animated_emoji,
			&Settings::suggestAnimatedEmoji, &Settings::setSuggestAnimatedEmoji),
		Field(u"device.cornerReaction"_q, tr::lng_settings_chat_corner_reaction,
			&Settings::cornerReaction, &Settings::setCornerReaction),
		Field(u"device.cornerReply"_q, tr::lng_settings_chat_corner_reply,
			&Settings::cornerReply, &Settings::setCornerReply),
		Field(u"device.pullToNextChannel"_q, tr::lng_settings_pull_to_next_channel,
			&Settings::pullToNextChannel, &Settings::setPullToNextChannel),
		Field(u"device.autoDownloadDictionaries"_q, tr::lng_settings_auto_download_dictionaries,
			&Settings::autoDownloadDictionaries, &Settings::setAutoDownloadDictionaries),
		Field(u"device.notifyFromAll"_q, tr::lng_settings_notify_all,
			&Settings::notifyFromAll, &Settings::setNotifyFromAll),
		Field(u"device.nativeWindowFrame"_q, tr::lng_settings_native_frame,
			&Settings::nativeWindowFrame, &Settings::setNativeWindowFrame),
		Field(u"device.systemAccentColorEnabled"_q, tr::lng_settings_theme_system_accent_color,
			&Settings::systemAccentColorEnabled, &Settings::setSystemAccentColorEnabled),
		Field(u"device.trayIconMonochrome"_q, tr::lng_settings_monochrome_icon,
			&Settings::trayIconMonochrome, &Settings::setTrayIconMonochrome),
		Field(u"device.translateButtonEnabled"_q, tr::lng_translate_settings_show,
			&Settings::translateButtonEnabled, &Settings::setTranslateButtonEnabled),
		Field(u"device.translateChatEnabled"_q, tr::lng_translate_settings_chat,
			&Settings::translateChatEnabled, &Settings::setTranslateChatEnabled),
		Field(u"device.chatFiltersHorizontal"_q, tr::lng_filters_horizontal,
			&Settings::chatFiltersHorizontal, &Settings::setChatFiltersHorizontal),
		Field(u"device.customFontFamily"_q, tr::lng_settings_font_family,
			&Settings::customFontFamily, &Settings::setCustomFontFamily),
		Field(u"device.songVolume"_q, tr::lng_serein_native_audio_volume,
			&Settings::songVolume, &Settings::setSongVolume,
			1.),
		Field(u"device.videoVolume"_q, tr::lng_serein_native_video_volume,
			&Settings::videoVolume, &Settings::setVideoVolume,
			1.),
		Field(u"device.notificationsVolume"_q, tr::lng_settings_master_volume_notifications,
			&Settings::notificationsVolume, &Settings::setNotificationsVolume,
			100),
		Field(u"device.notificationsCount"_q, tr::lng_settings_notifications_count,
			&Settings::notificationsCount, &Settings::setNotificationsCount,
			::Settings::kMaxNotificationsCount, 1),
		Choice(u"device.notifyView"_q, tr::lng_serein_native_preview,
			&Settings::notifyView, &Settings::setNotifyView,
			std::array{
				tr::lng_notification_show_text,
				tr::lng_notification_show_name,
				tr::lng_notification_hide_all,
			}),
		Choice(u"device.sendSubmitWay"_q, tr::lng_serein_native_send_key,
			&Settings::sendSubmitWay, &Settings::setSendSubmitWay,
			std::array{ tr::lng_settings_send_enter, tr::lng_settings_send_ctrlenter }),
	};
	return result;
}

[[nodiscard]] const auto &AccountSettings() {
	using Settings = Main::SessionSettings;
	using namespace Data::AutoDownload;
	static const auto result = [] {
		auto entries = std::vector{
			Field(u"account.archiveCollapsed"_q, tr::lng_serein_native_archive_collapsed,
				&Settings::archiveCollapsed, &Settings::setArchiveCollapsed),
			Field(u"account.archiveInMainMenu"_q, tr::lng_serein_native_archive_menu,
				&Settings::archiveInMainMenu, &Settings::setArchiveInMainMenu),
			Field(u"account.dialogsFiltersEnabled"_q, tr::lng_serein_native_chat_folders,
				&Settings::dialogsFiltersEnabled, &Settings::setDialogsFiltersEnabled),
		};
		const auto sources = std::array{
			std::tuple{ Source::User, "private", tr::lng_media_auto_in_private },
			std::tuple{ Source::Group, "groups", tr::lng_media_auto_in_groups },
			std::tuple{ Source::Channel, "channels", tr::lng_media_auto_in_channels },
		};
		const auto types = std::array{
			std::tuple{ Type::Photo, "photos", tr::lng_media_photo_title },
			std::tuple{ Type::File, "files", tr::lng_media_file_title },
			std::tuple{ Type::AutoPlayVideo, "video", tr::lng_media_video_title },
			std::tuple{ Type::AutoPlayVideoMessage, "roundVideo", tr::lng_media_video_messages_title },
			std::tuple{ Type::AutoPlayGIF, "gif", tr::lng_media_animation_title },
		};
		for (const auto &[source, group, groupTitle] : sources) {
			for (const auto &[type, name, typeTitle] : types) {
				entries.push_back(Setting<Settings>{
					.key = u"account.autoDownload."_q
						+ QLatin1String(group) + '.' + QLatin1String(name),
					.title = [=] {
						return groupTitle(tr::now) + u" · "_q + typeTitle(tr::now);
					},
					.read = [=](const Settings &owner) {
						return QJsonValue(qint64(owner.autoDownload().bytesLimit(source, type)));
					},
					.accepts = [](const QJsonValue &value) {
						const auto bytes = value.toInteger(-1);
						return bytes >= 0 && bytes <= kMaxBytesLimit;
					},
					.write = [=](Settings &owner, const QJsonValue &value) {
						owner.autoDownload().setBytesLimit(source, type, value.toInteger());
					},
					.format = [](const QJsonValue &value) {
						const auto bytes = value.toInteger();
						return bytes
							? Ui::FormatSizeText(bytes)
							: tr::lng_serein_config_off(tr::now);
					},
				});
			}
		}
		return entries;
	}();
	return result;
}

void Export(QJsonObject &result, const auto &owner, const auto &entries) {
	for (const auto &entry : entries) {
		result.insert(entry.key, entry.read(owner));
	}
}

bool CollectChanges(
		const auto &owner,
		const auto &entries,
		QJsonObject &remaining,
		QJsonObject &before,
		QJsonObject &after,
		QStringList &preview) {
	for (const auto &entry : entries) {
		const auto value = remaining.take(entry.key);
		if (value.isUndefined()) {
			continue;
		} else if (!entry.accepts(value)) {
			return false;
		}
		const auto current = entry.read(owner);
		if (current != value) {
			before.insert(entry.key, current);
			after.insert(entry.key, value);
			preview.push_back(entry.title() + u"\n"_q
				+ entry.format(current) + u" → "_q + entry.format(value));
		}
	}
	return true;
}

void Apply(auto &owner, const auto &entries, const QJsonObject &changes) {
	for (const auto &entry : entries) {
		const auto value = changes.value(entry.key);
		if (!value.isUndefined()) {
			entry.write(owner, value);
		}
	}
}

void RefreshNotifications(const QJsonObject &changes) {
	using Type = Window::Notifications::ChangeType;
	const auto types = std::array{
		std::pair{ u"device.soundNotify"_q, Type::SoundEnabled },
		std::pair{ u"device.desktopNotify"_q, Type::DesktopEnabled },
		std::pair{ u"device.flashBounceNotify"_q, Type::FlashBounceEnabled },
		std::pair{ u"device.includeMutedCounter"_q, Type::IncludeMuted },
		std::pair{ u"device.countUnreadMessages"_q, Type::CountMessages },
		std::pair{ u"device.notifyView"_q, Type::ViewParams },
		std::pair{ u"device.notificationsCount"_q, Type::MaxCount },
	};
	for (const auto &[key, type] : types) {
		if (changes.contains(key)) {
			Core::App().notifications().notifySettingsChanged(type);
		}
	}
}

void RefreshAutoDownload(
		not_null<Main::Session*> session,
		const QJsonObject &changes) {
	for (auto i = changes.begin(); i != changes.end(); ++i) {
		if (i.key().startsWith(u"account.autoDownload."_q)) {
			session->data().photoLoadSettingsChanged();
			session->data().documentLoadSettingsChanged();
			session->data().checkPlayingAnimations();
			break;
		}
	}
}

} // namespace

QJsonObject ExportNativeSettings(not_null<Main::Session*> session) {
	auto result = QJsonObject();
	Export(result, Core::App().settings(), DeviceSettings());
	Export(result, session->settings(), AccountSettings());
	return result;
}

NativeSettingsPlan PlanNativeSettings(
		not_null<Main::Session*> session,
		const QJsonObject &values) {
	auto result = NativeSettingsPlan();
	auto remaining = values;
	auto before = QJsonObject();
	auto after = QJsonObject();
	if (!CollectChanges(Core::App().settings(), DeviceSettings(),
			remaining, before, after, result.preview)
		|| !CollectChanges(session->settings(), AccountSettings(),
			remaining, before, after, result.preview)
		|| !remaining.isEmpty()) {
		return { .error = tr::lng_serein_native_invalid(tr::now) };
	}
	if (!after.isEmpty()) {
		result.unchanged = [=] {
			const auto current = ExportNativeSettings(session);
			for (auto i = before.begin(); i != before.end(); ++i) {
				if (current.value(i.key()) != i.value()) {
					return false;
				}
			}
			return true;
		};
		result.apply = [=] {
			Apply(Core::App().settings(), DeviceSettings(), after);
			Apply(session->settings(), AccountSettings(), after);
			RefreshNotifications(after);
			RefreshAutoDownload(session, after);
			Core::App().saveSettingsDelayed();
			session->saveSettingsDelayed();
		};
	}
	return result;
}

} // namespace Serein::Adapters
