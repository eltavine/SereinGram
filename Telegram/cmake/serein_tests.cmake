include(${CMAKE_CURRENT_LIST_DIR}/../SourceFiles/serein/schema/gen/sources.cmake)

set(serein_test_sources
    serein/tests/test_lang.cpp
    serein/tests/test_options.cpp
    serein/tests/test_spacing.cpp
    serein/tests/test_services.cpp
    serein/tests/test_filters.cpp
    serein/tests/test_links.cpp
    serein/tests/test_codec.cpp
    serein/tests/test_ghost.cpp
    serein/features/ghost/model/policy.cpp
    serein/features/history/model/recorder.cpp
    serein/schema/codec.cpp
    serein/chats/validators.cpp
    serein/compose/spacing.cpp
    serein/compose/validators.cpp
    serein/core/exchange.cpp
    serein/interface/main_menu_model.cpp
    serein/menu/model.cpp
    serein/services/model.cpp
    serein/filters/model.cpp
    serein/links/model.cpp
)
list(APPEND serein_test_sources ${serein_generated_sources})

set(serein_sql_test_sources
    serein/adapters/qtsql/history_store.cpp
    serein/tests/test_history_store.cpp
    serein/tests/test_history_recorder.cpp
)
