cmake_minimum_required(VERSION 3.25)
project(serein_core_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Sql)

get_filename_component(repo_loc "${CMAKE_CURRENT_SOURCE_DIR}/../../.." ABSOLUTE)
set(src_loc ${repo_loc}/Telegram/SourceFiles)
set(lib_loc ${repo_loc}/Telegram)

include(${lib_loc}/cmake/serein_tests.cmake)
list(APPEND serein_test_sources ${serein_sql_test_sources})
list(TRANSFORM serein_test_sources PREPEND ${src_loc}/)

add_executable(test_serein ${serein_test_sources} assertion_log.cpp)

target_include_directories(test_serein PRIVATE
    ${src_loc}
    ${lib_loc}/lib_ui
    ${lib_loc}/lib_base
    ${lib_loc}/lib_crl
    ${lib_loc}/lib_rpl
    ${lib_loc}/ThirdParty/GSL/include
    ${lib_loc}/ThirdParty/range-v3/include
    ${lib_loc}/ThirdParty/expected/include
)

target_link_libraries(test_serein PRIVATE Qt6::Core Qt6::Sql)

target_compile_definitions(test_serein PRIVATE
    SEREIN_LANG_SOURCE_DIR="${lib_loc}/Resources/langs"
    SEREIN_HAVE_QT_SQL
)

enable_testing()
add_test(NAME test_serein COMMAND test_serein)
