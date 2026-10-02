# Built from the pinned submodule: four C files and no configuration step.
set(serein_quirc_loc ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/quirc/lib)

add_library(serein_quirc STATIC
    ${serein_quirc_loc}/decode.c
    ${serein_quirc_loc}/identify.c
    ${serein_quirc_loc}/quirc.c
    ${serein_quirc_loc}/version_db.c
)
add_library(Serein::Quirc ALIAS serein_quirc)

target_include_directories(serein_quirc SYSTEM PUBLIC ${serein_quirc_loc})
# Busy screenshots have far more than the 254 regions of the default build.
target_compile_definitions(serein_quirc PUBLIC QUIRC_MAX_REGIONS=65534)
target_compile_options(serein_quirc PRIVATE $<IF:$<C_COMPILER_ID:MSVC>,/w,-w>)
set_target_properties(serein_quirc PROPERTIES COMPILE_WARNING_AS_ERROR OFF)
