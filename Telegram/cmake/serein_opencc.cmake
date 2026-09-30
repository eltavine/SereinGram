# Built from sources so no Python, CTest or dictionary compilation is needed.
set(serein_opencc_loc ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/OpenCC)
set(serein_opencc_src ${serein_opencc_loc}/src)
set(serein_marisa_loc ${serein_opencc_loc}/deps/marisa-0.3.1)
set(serein_opencc_gen ${CMAKE_CURRENT_BINARY_DIR}/serein_opencc)

add_library(serein_opencc STATIC)
add_library(Serein::OpenCC ALIAS serein_opencc)

target_sources(serein_opencc
PRIVATE
    ${serein_opencc_src}/BinaryDict.cpp
    ${serein_opencc_src}/Config.cpp
    ${serein_opencc_src}/Conversion.cpp
    ${serein_opencc_src}/ConversionAmbiguities.cpp
    ${serein_opencc_src}/ConversionCandidates.cpp
    ${serein_opencc_src}/ConversionChain.cpp
    ${serein_opencc_src}/Converter.cpp
    ${serein_opencc_src}/DartsDict.cpp
    ${serein_opencc_src}/Dict.cpp
    ${serein_opencc_src}/DictConverter.cpp
    ${serein_opencc_src}/DictEntry.cpp
    ${serein_opencc_src}/DictGroup.cpp
    ${serein_opencc_src}/Lexicon.cpp
    ${serein_opencc_src}/MarisaDict.cpp
    ${serein_opencc_src}/MaxMatchSegmentation.cpp
    ${serein_opencc_src}/PhraseExtract.cpp
    ${serein_opencc_src}/PipelineConverter.cpp
    ${serein_opencc_src}/PluginSegmentation.cpp
    ${serein_opencc_src}/PrefixMatch.cpp
    ${serein_opencc_src}/ResourceProvider.cpp
    ${serein_opencc_src}/Segmentation.cpp
    ${serein_opencc_src}/SerializableDict.cpp
    ${serein_opencc_src}/SerializedValues.cpp
    ${serein_opencc_src}/SimpleConverter.cpp
    ${serein_opencc_src}/SingleStageConverter.cpp
    ${serein_opencc_src}/TextDict.cpp
    ${serein_opencc_src}/UTF8StringSlice.cpp
    ${serein_opencc_src}/UTF8Util.cpp
    ${serein_marisa_loc}/lib/marisa/agent.cc
    ${serein_marisa_loc}/lib/marisa/keyset.cc
    ${serein_marisa_loc}/lib/marisa/trie.cc
    ${serein_marisa_loc}/lib/marisa/grimoire/io/mapper.cc
    ${serein_marisa_loc}/lib/marisa/grimoire/io/reader.cc
    ${serein_marisa_loc}/lib/marisa/grimoire/io/writer.cc
    ${serein_marisa_loc}/lib/marisa/grimoire/trie/louds-trie.cc
    ${serein_marisa_loc}/lib/marisa/grimoire/trie/tail.cc
    ${serein_marisa_loc}/lib/marisa/grimoire/vector/bit-vector.cc
)

include(GenerateExportHeader)
generate_export_header(serein_opencc
    BASE_NAME OPENCC
    EXPORT_MACRO_NAME OPENCC_EXPORT
    EXPORT_FILE_NAME ${serein_opencc_gen}/Opencc_Export.h
    STATIC_DEFINE Opencc_BUILT_AS_STATIC
)

target_include_directories(serein_opencc SYSTEM
PUBLIC
    ${serein_opencc_src}
    ${serein_opencc_gen}
PRIVATE
    ${serein_marisa_loc}/include
    ${serein_marisa_loc}/lib
    ${serein_opencc_loc}/deps/rapidjson-1.1.0
    ${serein_opencc_loc}/deps/darts-clone-0.32h/include
)

target_compile_definitions(serein_opencc
PUBLIC
    Opencc_BUILT_AS_STATIC
PRIVATE
    OPENCC_VERSION="1.4.2"
)

if (MSVC)
    target_compile_options(serein_opencc PRIVATE /utf-8)
    target_compile_definitions(serein_opencc PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
if (NOT WIN32)
    target_link_libraries(serein_opencc PRIVATE ${CMAKE_DL_LIBS})
endif()

set_target_properties(serein_opencc PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE ON
    COMPILE_WARNING_AS_ERROR OFF
)
