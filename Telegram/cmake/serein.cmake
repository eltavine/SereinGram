set(serein_sources
    serein/chats/layout.cpp
    serein/chats/list_refresher.cpp
    serein/chats/promotions.cpp
    serein/compose/buttons.cpp
    serein/compose/confirm.cpp
    serein/compose/format_toolbar.cpp
    serein/compose/forward_options.cpp
    serein/compose/mention.cpp
    serein/compose/mention_query.cpp
    serein/compose/placeholder.cpp
    serein/compose/spacing.cpp
    serein/compose/text.cpp
    serein/compose/text_replacements.cpp
    serein/compose/validators.cpp
    serein/app/language.cpp
    serein/core/exchange.cpp
    serein/app/options.cpp
    serein/adapters/tdesktop/prefs.cpp
    serein/schema/codec.cpp
    serein/features/ghost/model/policy.cpp
    serein/features/ghost/model/exceptions.cpp
    serein/features/history/deleted_marks.cpp
    serein/features/history/media_store.cpp
    serein/features/history/model/recorder.cpp
    serein/features/updates/model/release.cpp
    serein/features/regdate/model/estimate.cpp
    serein/adapters/openssl/aes_gcm_cipher.cpp
    serein/features/ghost/hooks.cpp
    serein/features/ghost/read_until_here.cpp
    serein/features/ghost/story_tip.cpp
    serein/app/ghost_read.cpp
    serein/features/history/entities.cpp
    serein/features/history/menu.cpp
    serein/app/history_expiry.cpp
    serein/app/history_fade.cpp
    serein/app/history_restore.cpp
    serein/app/lifecycle.cpp
    serein/app/main_menu.cpp
    serein/app/message_menu.cpp
    serein/app/compose_field.cpp
    serein/app/modules.cpp
    serein/app/peer_menu.cpp
    serein/app/recent_chats.cpp
    serein/app/send_options.cpp
    serein/app/shortcuts.cpp
    serein/app/sticker_set_menu.cpp
    serein/app/user_lookup.cpp
    serein/features/history/bubbles.cpp
    serein/features/history/restored_message.cpp
    serein/features/history/viewer.cpp
    serein/app/tray_menu.cpp
    serein/app/updates.cpp
    serein/app/ghost_menu.cpp
    serein/app/ghost_send.cpp
    serein/display/peer_id.cpp
    serein/display/view_refresher.cpp
    serein/interface/main_menu.cpp
    serein/interface/app_icon.cpp
    serein/interface/main_menu_model.cpp
    serein/interface/notifications.cpp
    serein/interface/text.cpp
    serein/interface/reply_colors.cpp
    serein/interface/global_shortcut.cpp
    serein/chats/startup_folder.cpp
    serein/chats/sort.cpp
    serein/chats/validators.cpp
    serein/chats/recent.cpp
    serein/chats/reading_positions.cpp
    serein/chats/quick_actions.cpp
    serein/chats/chat_cache.cpp
    serein/chats/local_pins.cpp
    serein/app/reading_positions.cpp
    serein/chats/managed_folders.cpp
    serein/chats/shown_filters.cpp
    serein/chats/shown_order.cpp
    serein/interface/roundness.cpp
    serein/settings/interface.cpp
    serein/settings/restart.cpp
    serein/settings/rows.cpp
    serein/settings/ghost_exceptions.cpp
    serein/settings/subpages.cpp
    serein/app/auto_demo.cpp
    serein/admin/delete_mine.cpp
    serein/admin/unblock_all.cpp
    serein/admin/upgrade.cpp
    serein/admin/shortcuts.cpp
    serein/privacy/recorders.cpp
    serein/privacy/recorders_platform.cpp
    serein/menu/actions.cpp
    serein/menu/contributors.cpp
    serein/menu/batch.cpp
    serein/menu/rating.cpp
    serein/menu/media.cpp
    serein/menu/buttons.cpp
    serein/menu/details.cpp
    serein/menu/model.cpp
    serein/menu/repeat.cpp
    serein/menu/reminder.cpp
    serein/menu/selection.cpp
    serein/media/sticker_catalog.cpp
    serein/media/sticker_catalog_rules.cpp
    serein/media/chat_downloads.cpp
    serein/media/download_names.cpp
    serein/media/force_preview.cpp
    serein/media/sticker_rounding.cpp
    serein/media/voice_denoise.cpp
    serein/messages/format.cpp
    serein/messages/content.cpp
    serein/messages/dates.cpp
    serein/messages/persian_calendar.cpp
    serein/messages/selection_limit.cpp
    serein/messages/double_click.cpp
    serein/messages/badges.cpp
    serein/messages/effects.cpp
    serein/messages/reactions.cpp
    serein/messages/chinese.cpp
    serein/messages/reading.cpp
    serein/messages/reading_menu.cpp
    serein/messages/chinese_warmup.cpp
    serein/tests/menu_scenario.cpp
    serein/filters/model.cpp
    serein/filters/hidden_messages.cpp
    serein/filters/reveal.cpp
    serein/filters/view.cpp
    serein/filters/settings.cpp
    serein/filters/subscription.cpp
    serein/filters/hide_message_menu.cpp
    serein/filters/menu.cpp
    serein/links/model.cpp
    serein/links/open.cpp
    serein/links/settings.cpp
    serein/snapshot/rules.cpp
    serein/snapshot/snapshot.cpp
    serein/privacy/profile.cpp
    serein/privacy/sessions.cpp
    serein/privacy/alias.cpp
    serein/privacy/alias_model.cpp
    serein/privacy/alias_rules.cpp
    serein/settings/home.cpp
    serein/settings/lock.cpp
    serein/settings/rules.cpp
    serein/settings/chats.cpp
    serein/settings/compose.cpp
    serein/settings/config.cpp
    serein/settings/media.cpp
    serein/settings/menu.cpp
    serein/settings/privacy.cpp
    serein/settings/messages.cpp
    serein/settings/services.cpp
    serein/settings/services_network.cpp
    serein/services/credentials.cpp
    serein/services/model.cpp
    serein/services/translation_protocol.cpp
    serein/services/store.cpp
    serein/adapters/qtnetwork/manager.cpp
    serein/services/request.cpp
    serein/services/translation.cpp
    serein/services/web_apps.cpp
    serein/links/preview.cpp
    serein/services/translation_context.cpp
    serein/services/draft_translation.cpp
    serein/services/summary.cpp
    serein/services/summary_protocol.cpp
    serein/network/doh.cpp
    serein/network/proxy_import.cpp
    serein/network/proxy_order.cpp
    serein/network/proxy_notes.cpp
    serein/network/proxy_note.cpp
    serein/network/proxy_subscription.cpp
    serein/network/proxy_tools.cpp
    serein/network/vpn_proxy.cpp
    serein/network/vpn_rules.cpp
    serein/services/transcription.cpp
    serein/services/system_ai.cpp
)

include(${CMAKE_CURRENT_LIST_DIR}/../SourceFiles/serein/schema/gen/sources.cmake)
list(APPEND serein_sources
    ${serein_generated_sources}
    ${serein_generated_hook_sources}
)

if (WIN32)
    list(APPEND serein_sources serein/adapters/credentials/wincred.cpp)
elseif (APPLE)
    list(APPEND serein_sources serein/adapters/credentials/keychain.cpp)
else()
    list(APPEND serein_sources serein/adapters/credentials/local_file.cpp)
endif()

if (serein_sources)
    nice_target_sources(Telegram ${src_loc} PRIVATE ${serein_sources})
endif()

find_package(Qt6 QUIET COMPONENTS Sql)
if (TARGET Qt6::Sql)
    message(STATUS "Serein: Qt Sql found, message history enabled.")
    nice_target_sources(Telegram ${src_loc} PRIVATE
        serein/adapters/qtsql/history_store.cpp
        serein/app/history_hooks.cpp
    )
    target_link_libraries(Telegram PRIVATE Qt6::Sql)
    if (TARGET Qt6::QSQLiteDriverPlugin)
        qt_import_plugins(Telegram
            INCLUDE_BY_TYPE sqldrivers Qt6::QSQLiteDriverPlugin)
    endif()
else()
    message(STATUS "Serein: Qt Sql not found, message history disabled.")
    nice_target_sources(Telegram ${src_loc} PRIVATE serein/app/history_hooks_disabled.cpp)
endif()

if (APPLE AND NOT DESKTOP_APP_DISABLE_SWIFT6)
    enable_language(Swift)
    set_target_properties(Telegram PROPERTIES LINKER_LANGUAGE CXX)
    set(serein_swift_deployment "${CMAKE_OSX_DEPLOYMENT_TARGET}")
    if (NOT serein_swift_deployment OR serein_swift_deployment VERSION_LESS 11.0)
        set(serein_swift_deployment 11.0)
    endif()
    if (NOT CMAKE_GENERATOR STREQUAL "Xcode")
        set(serein_swift_arch "${CMAKE_OSX_ARCHITECTURES}")
        if (NOT serein_swift_arch)
            set(serein_swift_arch "${CMAKE_SYSTEM_PROCESSOR}")
        endif()
        list(LENGTH serein_swift_arch serein_swift_arch_count)
        if (NOT serein_swift_arch_count EQUAL 1)
            message(FATAL_ERROR "Use Xcode for a universal Swift build, or select one CMAKE_OSX_ARCHITECTURES value.")
        endif()
    endif()
    function(serein_configure_swift_target target_name)
        if (CMAKE_GENERATOR STREQUAL "Xcode")
            set_target_properties(${target_name} PROPERTIES
                XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "${serein_swift_deployment}")
        else()
            target_compile_options(${target_name} PRIVATE
                "-target" "${serein_swift_arch}-apple-macos${serein_swift_deployment}")
        endif()
    endfunction()
    serein_configure_swift_target(lib_translate)
    find_library(SEREIN_FOUNDATION_MODELS FoundationModels)
    if (SEREIN_FOUNDATION_MODELS)
        add_library(serein_system_ai STATIC
            ${src_loc}/serein/services/system_ai.swift)
        set_target_properties(serein_system_ai PROPERTIES
            Swift_LANGUAGE_VERSION 6
            Swift_COMPILATION_MODE wholemodule)
        serein_configure_swift_target(serein_system_ai)
        target_link_options(serein_system_ai INTERFACE
            "SHELL:-weak_framework FoundationModels")
        target_link_libraries(Telegram PRIVATE serein_system_ai)
        target_compile_definitions(Telegram PRIVATE SEREIN_SYSTEM_AI)
    endif()
endif()

nice_target_sources(Telegram ${res_loc} PRIVATE qrc/serein.qrc qrc/serein_opencc.qrc)

file(GLOB serein_lang_files CONFIGURE_DEPENDS ${res_loc}/langs/serein/*.strings)
list(FILTER serein_lang_files EXCLUDE REGEX "/serein\\.strings$")
list(SORT serein_lang_files)
set(serein_langs_entries "")
foreach (serein_lang_file ${serein_lang_files})
    get_filename_component(serein_lang_name ${serein_lang_file} NAME)
    string(APPEND serein_langs_entries
        "    <file alias=\"${serein_lang_name}\">${serein_lang_file}</file>\n")
endforeach()
set(serein_langs_qrc ${CMAKE_CURRENT_BINARY_DIR}/serein_langs.qrc)
set(serein_langs_new "<RCC>\n  <qresource prefix=\"/langs/serein\">\n${serein_langs_entries}  </qresource>\n</RCC>\n")
set(serein_langs_old "")
if (EXISTS "${serein_langs_qrc}")
    file(READ "${serein_langs_qrc}" serein_langs_old)
endif()
if (NOT "${serein_langs_new}" STREQUAL "${serein_langs_old}")
    file(WRITE "${serein_langs_qrc}" "${serein_langs_new}")
endif()
set_source_files_properties("${serein_langs_qrc}" PROPERTIES
    QRC_GENERATED_FROM "${serein_lang_files}")
nice_target_sources(Telegram ${CMAKE_CURRENT_BINARY_DIR} PRIVATE serein_langs.qrc)

option(SEREIN_USE_SYSTEM_OPENCC "Link the system OpenCC instead of the bundled submodule." OFF)
if (SEREIN_USE_SYSTEM_OPENCC)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SEREIN_OPENCC REQUIRED IMPORTED_TARGET opencc)
    add_library(serein_opencc_system INTERFACE)
    target_link_libraries(serein_opencc_system INTERFACE PkgConfig::SEREIN_OPENCC)
    add_library(Serein::OpenCC ALIAS serein_opencc_system)
else()
    include(${CMAKE_CURRENT_LIST_DIR}/serein_opencc.cmake)
endif()
target_link_libraries(Telegram PRIVATE Serein::OpenCC)

if (TARGET desktop-app::external_rnnoise)
    target_link_libraries(Telegram PRIVATE desktop-app::external_rnnoise)
    target_compile_definitions(Telegram PRIVATE SEREIN_HAVE_RNNOISE)
endif()

if (DESKTOP_APP_USE_PACKAGED)
    target_compile_definitions(Telegram PRIVATE SEREIN_SYSTEM_PACKAGE)
endif()

if (WIN32)
    target_link_libraries(Telegram PRIVATE Dnsapi)
endif()

if (DESKTOP_APP_TEST_APPS)
    add_executable(test_serein)
    init_target(test_serein "(tests)")

    include(cmake/serein_tests.cmake)
    nice_target_sources(test_serein ${src_loc} PRIVATE ${serein_test_sources})

    find_package(Qt6 QUIET COMPONENTS Sql)
    if (TARGET Qt6::Sql)
        message(STATUS "Serein: Qt Sql found, history store tests enabled.")
        nice_target_sources(test_serein ${src_loc} PRIVATE ${serein_sql_test_sources})
        target_link_libraries(test_serein PRIVATE Qt6::Sql)
        target_compile_definitions(test_serein PRIVATE SEREIN_HAVE_QT_SQL)
        if (TARGET Qt6::QSQLiteDriverPlugin)
            qt_import_plugins(test_serein
                INCLUDE_BY_TYPE sqldrivers Qt6::QSQLiteDriverPlugin)
        endif()
    else()
        message(STATUS "Serein: Qt Sql not found, history store tests skipped.")
    endif()

    target_include_directories(test_serein PRIVATE
        ${src_loc}
        ${CMAKE_CURRENT_SOURCE_DIR}/lib_ui)

    target_link_libraries(test_serein PRIVATE
        desktop-app::lib_base
        desktop-app::lib_crl
        desktop-app::external_openssl
        desktop-app::external_qt
        Serein::OpenCC
    )

    target_compile_definitions(test_serein PRIVATE
        SEREIN_LANG_SOURCE_DIR="${res_loc}/langs"
        SEREIN_OPENCC_DICTIONARY_DIR="${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/OpenCC/data/dictionary"
        SEREIN_REGDATE_POINTS="${res_loc}/serein/regdate_points.json"
    )

    set_target_properties(test_serein PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/serein-tests/$<CONFIG>"
    )
endif()
