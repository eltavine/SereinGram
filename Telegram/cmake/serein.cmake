set(serein_sources
    serein/chats/layout.cpp
    serein/chats/list_refresher.cpp
    serein/chats/promotions.cpp
    serein/compose/confirm.cpp
    serein/compose/placeholder.cpp
    serein/compose/spacing.cpp
    serein/compose/text.cpp
    serein/compose/validators.cpp
    serein/core/language.cpp
    serein/core/exchange.cpp
    serein/core/options.cpp
    serein/schema/codec.cpp
    serein/features/ghost/model/policy.cpp
    serein/features/history/model/recorder.cpp
    serein/adapters/openssl/aes_gcm_cipher.cpp
    serein/hooks/ghost.cpp
    serein/app/ghost_read.cpp
    serein/app/ghost_menu.cpp
    serein/app/ghost_send.cpp
    serein/display/view_refresher.cpp
    serein/interface/main_menu.cpp
    serein/interface/main_menu_model.cpp
    serein/interface/notifications.cpp
    serein/interface/text.cpp
    serein/chats/startup_folder.cpp
    serein/chats/sort.cpp
    serein/chats/validators.cpp
    serein/chats/managed_folders.cpp
    serein/interface/roundness.cpp
    serein/settings/interface.cpp
    serein/settings/restart.cpp
    serein/menu/actions.cpp
    serein/menu/batch.cpp
    serein/menu/media.cpp
    serein/menu/history.cpp
    serein/menu/model.cpp
    serein/menu/repeat.cpp
    serein/menu/reading.cpp
    serein/menu/selection.cpp
	serein/media/sticker_catalog.cpp
    serein/messages/format.cpp
    serein/messages/content.cpp
    serein/messages/badges.cpp
    serein/messages/effects.cpp
    serein/messages/reactions.cpp
    serein/messages/reading.cpp
    serein/tests/menu_scenario.cpp
    serein/filters/model.cpp
    serein/filters/view.cpp
    serein/filters/settings.cpp
    serein/filters/menu.cpp
    serein/links/model.cpp
    serein/links/open.cpp
    serein/links/settings.cpp
    serein/snapshot/snapshot.cpp
    serein/privacy/profile.cpp
    serein/privacy/alias.cpp
    serein/privacy/alias_model.cpp
    serein/settings/home.cpp
    serein/settings/rules.cpp
    serein/settings/chats.cpp
    serein/settings/compose.cpp
	serein/settings/config.cpp
    serein/settings/media.cpp
    serein/settings/menu.cpp
    serein/settings/privacy.cpp
    serein/settings/messages.cpp
    serein/settings/services.cpp
    serein/services/credentials.cpp
    serein/services/model.cpp
    serein/services/store.cpp
    serein/services/request.cpp
    serein/services/translation.cpp
    serein/services/draft_translation.cpp
    serein/services/transcription.cpp
    serein/services/system_ai.cpp
)

include(${CMAKE_CURRENT_LIST_DIR}/../SourceFiles/serein/schema/gen/sources.cmake)
list(APPEND serein_sources ${serein_generated_sources})

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
        qt_import_plugins(Telegram INCLUDE Qt6::QSQLiteDriverPlugin)
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

nice_target_sources(Telegram ${res_loc} PRIVATE qrc/serein.qrc)

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
            qt_import_plugins(test_serein INCLUDE Qt6::QSQLiteDriverPlugin)
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
    )

    target_compile_definitions(test_serein PRIVATE
        SEREIN_LANG_SOURCE_DIR="${res_loc}/langs"
    )

    set_target_properties(test_serein PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/serein-tests/$<CONFIG>"
    )

    add_dependencies(Telegram test_serein)
endif()
