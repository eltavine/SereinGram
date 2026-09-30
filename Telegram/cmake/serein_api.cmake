# SereinGram: api_id and api_hash come from the environment
# (SEREIN_API_ID, SEREIN_API_HASH) or from the uncommitted file
# Telegram/build/api_credentials.local.cmake. Both are ignored with
# -D TDESKTOP_API_TEST=ON; explicit -D TDESKTOP_API_ID / TDESKTOP_API_HASH
# values are used only while neither source is set.

set(serein_api_local ${CMAKE_CURRENT_SOURCE_DIR}/build/api_credentials.local.cmake)
if (EXISTS ${serein_api_local})
    include(${serein_api_local})
endif()
if (DEFINED ENV{SEREIN_API_ID} AND NOT "$ENV{SEREIN_API_ID}" STREQUAL "")
    set(SEREIN_API_ID "$ENV{SEREIN_API_ID}")
endif()
if (DEFINED ENV{SEREIN_API_HASH} AND NOT "$ENV{SEREIN_API_HASH}" STREQUAL "")
    set(SEREIN_API_HASH "$ENV{SEREIN_API_HASH}")
endif()

if (NOT TDESKTOP_API_TEST
    AND (NOT TDESKTOP_API_ID OR TDESKTOP_API_ID STREQUAL "0" OR SEREIN_API_FROM_LOCAL)
    AND DEFINED SEREIN_API_ID)
    if (NOT SEREIN_API_ID MATCHES "^[1-9][0-9]*$")
        message(FATAL_ERROR "SEREIN_API_ID must be a positive integer.")
    endif()
    string(LENGTH "${SEREIN_API_HASH}" serein_api_hash_length)
    if (NOT SEREIN_API_HASH MATCHES "^[0-9a-f]+$"
        OR NOT serein_api_hash_length EQUAL 32)
        message(FATAL_ERROR "SEREIN_API_HASH must be 32 lowercase hex digits.")
    endif()
    set(TDESKTOP_API_ID ${SEREIN_API_ID} CACHE STRING "Provide 'api_id' for the Telegram API access." FORCE)
    set(TDESKTOP_API_HASH ${SEREIN_API_HASH} CACHE STRING "Provide 'api_hash' for the Telegram API access." FORCE)
    set(SEREIN_API_FROM_LOCAL ON CACHE INTERNAL "api_id and api_hash were taken from SEREIN_API_*.")
    message(STATUS "Serein: using api_id ${SEREIN_API_ID} from the environment or local credentials file.")
endif()

# SereinGram updates through GitHub Releases, never Telegram's update channel.
set(DESKTOP_APP_DISABLE_AUTOUPDATE ON CACHE BOOL "Disable autoupdate." FORCE)
