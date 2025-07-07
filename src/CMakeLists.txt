cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project(bowsie VERSION 1.10 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

add_subdirectory("include")

add_executable(bowsie 
    "bowsie.cpp"
    "include/misc.cpp"
    "include/map16.cpp"
    "include/rom.cpp"
    "include/settings.cpp"
    "include/meowmeow/meowmeow.cpp"
    "resources/bowsie.rc"
    "include/meowmeow/meowmeow.cpp"
)

target_link_libraries(bowsie
    PRIVATE fmt::fmt
    PRIVATE asar-static
    PRIVATE nlohmann_json::nlohmann_json
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND LINUX)
    target_compile_options(bowsie PRIVATE -fsanitize=address)
    target_link_options(bowsie PRIVATE -fsanitize=address)
endif()

