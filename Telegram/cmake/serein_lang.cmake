# SereinGram: the SereinGram strings join the generated language keys, and
# the generated key lookup is compiled as one function per key letter
# because MSVC for arm64 rejects the single function as too large.

set(serein_lang_sources
    ${res_loc}/langs/lang.strings
    ${res_loc}/langs/serein/serein.strings
)
set(serein_lang_gen ${CMAKE_CURRENT_BINARY_DIR}/gen)
set(serein_lang_merged ${serein_lang_gen}/lang_merged.strings)
set(serein_lang_content "")
foreach (serein_lang_source ${serein_lang_sources})
    file(READ ${serein_lang_source} serein_lang_part)
    string(APPEND serein_lang_content "${serein_lang_part}\n")
endforeach()
file(WRITE ${serein_lang_merged}.tmp "${serein_lang_content}")
configure_file(${serein_lang_merged}.tmp ${serein_lang_merged} COPYONLY)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${serein_lang_sources})

generate_lang(td_lang ${serein_lang_merged} ${src_loc})

find_package(Python3 REQUIRED)
set(serein_lang_split ${serein_lang_gen}/serein_lang_auto.cpp)
set(serein_lang_script ${CMAKE_CURRENT_LIST_DIR}/../../tools/serein/split_lang_keys.py)
add_custom_command(
OUTPUT
    ${serein_lang_split}
COMMAND
    ${Python3_EXECUTABLE}
    ${serein_lang_script}
    ${serein_lang_gen}/lang_auto.cpp
    ${serein_lang_split}
COMMENT "Splitting the lang key lookup (td_lang)"
DEPENDS
    ${serein_lang_gen}/lang_auto.timestamp
    ${serein_lang_script}
)
set_source_files_properties(${serein_lang_gen}/lang_auto.cpp PROPERTIES HEADER_FILE_ONLY ON)
target_sources(td_lang PRIVATE ${serein_lang_split})
source_group("(gen)" FILES ${serein_lang_split})
