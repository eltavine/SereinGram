cmake_minimum_required(VERSION 3.25)
project(serein_core_tests LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Sql)
find_package(OpenSSL 3 REQUIRED)

get_filename_component(repo_loc "${CMAKE_CURRENT_SOURCE_DIR}/../../.." ABSOLUTE)
set(src_loc ${repo_loc}/Telegram/SourceFiles)
set(lib_loc ${repo_loc}/Telegram)

include(${lib_loc}/cmake/serein_tests.cmake)
include(${lib_loc}/cmake/serein_opencc.cmake)
include(${lib_loc}/cmake/serein_quirc.cmake)
list(APPEND serein_test_sources ${serein_sql_test_sources})
list(TRANSFORM serein_test_sources PREPEND ${src_loc}/)

set_source_files_properties(${lib_loc}/ThirdParty/QR/cpp/qrcodegen.cpp
    PROPERTIES COMPILE_OPTIONS -w)
add_executable(test_serein
    ${serein_test_sources}
    ${lib_loc}/ThirdParty/QR/cpp/qrcodegen.cpp
    assertion_log.cpp)

target_include_directories(test_serein PRIVATE
    ${src_loc}
    ${lib_loc}/lib_ui
    ${lib_loc}/lib_base
    ${lib_loc}/lib_crl
    ${lib_loc}/lib_rpl
    ${lib_loc}/ThirdParty/GSL/include
    ${lib_loc}/ThirdParty/range-v3/include
    ${lib_loc}/ThirdParty/expected/include
    ${lib_loc}/ThirdParty/doctest
    ${lib_loc}/ThirdParty/QR/cpp
)

target_link_libraries(test_serein PRIVATE
    Qt6::Core
    Qt6::Sql
    OpenSSL::Crypto
    Serein::OpenCC
    Serein::Quirc)

target_compile_definitions(test_serein PRIVATE
    SEREIN_LANG_SOURCE_DIR="${lib_loc}/Resources/langs"
    SEREIN_OPENCC_DICTIONARY_DIR="${lib_loc}/ThirdParty/OpenCC/data/dictionary"
    SEREIN_REGDATE_POINTS="${lib_loc}/Resources/serein/regdate_points.json"
    SEREIN_PRESETS_DIR="${lib_loc}/Resources/serein/presets"
    SEREIN_RELEASE_FIXTURE="${repo_loc}/tools/serein/tests/fixtures/release_manifest.json"
    SEREIN_HAVE_QT_SQL
)

enable_testing()
add_test(NAME test_serein COMMAND test_serein)
