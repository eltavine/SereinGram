set(nagram_sources)

if (nagram_sources)
    nice_target_sources(Telegram ${src_loc} PRIVATE ${nagram_sources})
endif()

if (DESKTOP_APP_TEST_APPS)
    add_executable(test_nagram)
    init_target(test_nagram "(tests)")

    nice_target_sources(test_nagram ${src_loc} PRIVATE
        nagram/tests/test_lang.cpp
    )

    target_compile_definitions(test_nagram PRIVATE
        NAGRAM_LANG_SOURCE_DIR="${res_loc}/langs"
    )

    set_target_properties(test_nagram PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/nagram-tests/$<CONFIG>"
    )

    add_dependencies(Telegram test_nagram)
endif()
