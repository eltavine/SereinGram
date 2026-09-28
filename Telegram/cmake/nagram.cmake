set(nagram_sources
    nagram/chats/layout.cpp
    nagram/chats/list_refresher.cpp
    nagram/chats/promotions.cpp
    nagram/compose/confirm.cpp
    nagram/compose/placeholder.cpp
    nagram/compose/spacing.cpp
    nagram/compose/text.cpp
    nagram/core/language.cpp
    nagram/core/exchange.cpp
    nagram/core/options.cpp
    nagram/display/view_refresher.cpp
    nagram/interface/main_menu.cpp
    nagram/interface/main_menu_model.cpp
    nagram/interface/notifications.cpp
    nagram/interface/text.cpp
    nagram/chats/startup_folder.cpp
    nagram/chats/sort.cpp
    nagram/chats/managed_folders.cpp
    nagram/interface/roundness.cpp
    nagram/settings/interface.cpp
    nagram/settings/restart.cpp
    nagram/menu/actions.cpp
    nagram/menu/batch.cpp
    nagram/menu/media.cpp
    nagram/menu/model.cpp
    nagram/menu/repeat.cpp
    nagram/menu/reading.cpp
    nagram/menu/selection.cpp
    nagram/messages/format.cpp
    nagram/messages/content.cpp
    nagram/messages/badges.cpp
    nagram/messages/effects.cpp
    nagram/messages/reactions.cpp
    nagram/messages/reading.cpp
    nagram/tests/menu_scenario.cpp
    nagram/privacy/profile.cpp
    nagram/settings/home.cpp
    nagram/settings/chats.cpp
    nagram/settings/compose.cpp
    nagram/settings/media.cpp
    nagram/settings/menu.cpp
    nagram/settings/privacy.cpp
    nagram/settings/messages.cpp
    nagram/settings/services.cpp
    nagram/services/credentials.cpp
    nagram/services/model.cpp
    nagram/services/store.cpp
    nagram/services/request.cpp
    nagram/services/translation.cpp
    nagram/services/draft_translation.cpp
    nagram/services/transcription.cpp
    nagram/services/system_ai.cpp
)

if (nagram_sources)
    nice_target_sources(Telegram ${src_loc} PRIVATE ${nagram_sources})
endif()

if (APPLE AND NOT DESKTOP_APP_DISABLE_SWIFT6)
    enable_language(Swift)
    set_target_properties(Telegram PROPERTIES LINKER_LANGUAGE CXX)
    set(nagram_swift_deployment "${CMAKE_OSX_DEPLOYMENT_TARGET}")
    if (NOT nagram_swift_deployment OR nagram_swift_deployment VERSION_LESS 11.0)
        set(nagram_swift_deployment 11.0)
    endif()
    if (NOT CMAKE_GENERATOR STREQUAL "Xcode")
        set(nagram_swift_arch "${CMAKE_OSX_ARCHITECTURES}")
        if (NOT nagram_swift_arch)
            set(nagram_swift_arch "${CMAKE_SYSTEM_PROCESSOR}")
        endif()
        list(LENGTH nagram_swift_arch nagram_swift_arch_count)
        if (NOT nagram_swift_arch_count EQUAL 1)
            message(FATAL_ERROR "Use Xcode for a universal Swift build, or select one CMAKE_OSX_ARCHITECTURES value.")
        endif()
    endif()
    function(nagram_configure_swift_target target_name)
        if (CMAKE_GENERATOR STREQUAL "Xcode")
            set_target_properties(${target_name} PROPERTIES
                XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "${nagram_swift_deployment}")
        else()
            target_compile_options(${target_name} PRIVATE
                "-target" "${nagram_swift_arch}-apple-macos${nagram_swift_deployment}")
        endif()
    endfunction()
    nagram_configure_swift_target(lib_translate)
    find_library(NAGRAM_FOUNDATION_MODELS FoundationModels)
    if (NAGRAM_FOUNDATION_MODELS)
        add_library(nagram_system_ai STATIC
            ${src_loc}/nagram/services/system_ai.swift)
        set_target_properties(nagram_system_ai PROPERTIES
            Swift_LANGUAGE_VERSION 6
            Swift_COMPILATION_MODE wholemodule)
        nagram_configure_swift_target(nagram_system_ai)
        target_link_options(nagram_system_ai INTERFACE
            "SHELL:-weak_framework FoundationModels")
        target_link_libraries(Telegram PRIVATE nagram_system_ai)
        target_compile_definitions(Telegram PRIVATE NAGRAM_SYSTEM_AI)
    endif()
endif()

nice_target_sources(Telegram ${res_loc} PRIVATE qrc/nagram.qrc)

if (DESKTOP_APP_TEST_APPS)
    add_executable(test_nagram)
    init_target(test_nagram "(tests)")

    nice_target_sources(test_nagram ${src_loc} PRIVATE
        nagram/tests/test_lang.cpp
        nagram/tests/test_options.cpp
        nagram/tests/test_spacing.cpp
        nagram/tests/test_services.cpp
        nagram/compose/spacing.cpp
        nagram/core/exchange.cpp
        nagram/interface/main_menu_model.cpp
        nagram/menu/model.cpp
        nagram/services/model.cpp
    )

    target_include_directories(test_nagram PRIVATE ${src_loc})

    target_link_libraries(test_nagram PRIVATE
        desktop-app::lib_base
        desktop-app::lib_crl
        desktop-app::external_qt
    )

    target_compile_definitions(test_nagram PRIVATE
        NAGRAM_LANG_SOURCE_DIR="${res_loc}/langs"
    )

    set_target_properties(test_nagram PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/nagram-tests/$<CONFIG>"
    )

    add_dependencies(Telegram test_nagram)
endif()
