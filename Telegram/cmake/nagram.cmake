set(nagram_sources
    nagram/chats/layout.cpp
    nagram/chats/list_refresher.cpp
    nagram/chats/promotions.cpp
    nagram/core/language.cpp
    nagram/core/options.cpp
    nagram/display/view_refresher.cpp
    nagram/messages/format.cpp
    nagram/messages/content.cpp
    nagram/messages/badges.cpp
    nagram/messages/effects.cpp
    nagram/messages/reactions.cpp
    nagram/settings/home.cpp
    nagram/settings/chats.cpp
    nagram/settings/messages.cpp
)

if (nagram_sources)
    nice_target_sources(Telegram ${src_loc} PRIVATE ${nagram_sources})
endif()

nice_target_sources(Telegram ${res_loc} PRIVATE qrc/nagram.qrc)

if (DESKTOP_APP_TEST_APPS)
    add_executable(test_nagram)
    init_target(test_nagram "(tests)")

    nice_target_sources(test_nagram ${src_loc} PRIVATE
        nagram/tests/test_lang.cpp
        nagram/tests/test_options.cpp
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
