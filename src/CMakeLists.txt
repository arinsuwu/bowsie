cmake_minimum_required(VERSION 4.1.0-rc1 FATAL_ERROR)

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
set(CMAKE_CXX_MODULE_STD ON)

project(bowsie VERSION 1.10 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS ON)

add_subdirectory(include)

add_executable(bowsie)
target_sources(bowsie
    PUBLIC bowsie.cpp
    PUBLIC include/misc.cpp
    PUBLIC include/map16.cpp
    PUBLIC include/rom.cpp
    PUBLIC include/settings.cpp
    PUBLIC resources/bowsie.rc
)
target_link_libraries(bowsie
    PRIVATE asar
    PRIVATE rapidjson
    PRIVATE meowmeow
)

