cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project(bowsie VERSION 1.20 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

set(BOWSIE_LIBRARIES "")
set(BOWSIE_SOURCE "")

add_subdirectory("include")

list(APPEND BOWSIE_SOURCE
    "bowsie.cpp"

    "include/misc.cpp"
    "include/map16.cpp"
    "include/rom.cpp"
    "include/settings.cpp"
    "include/meowmeow/meowmeow.cpp"

    "resources/bowsie.rc"
)

if(ASAR_DYNAMIC_LINK)
    list(APPEND BOWSIE_SOURCE
        "include/asar/asardll.h"
        "include/asar/asardll.c"
    )
endif()

add_executable(bowsie ${BOWSIE_SOURCE})

list(APPEND BOWSIE_LIBRARIES PRIVATE fmt::fmt PRIVATE nlohmann_json::nlohmann_json)
if(NOT ASAR_DYNAMIC_LINK)
    target_link_libraries(bowsie PRIVATE asar-static)
else()
    target_compile_definitions(bowsie PRIVATE ASAR_DYNAMIC_LINK)
endif()

target_link_libraries(bowsie ${BOWSIE_LIBRARIES})

if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND LINUX)
    target_compile_options(bowsie PRIVATE -fsanitize=address)
    target_link_options(bowsie PRIVATE -fsanitize=address)
endif()

