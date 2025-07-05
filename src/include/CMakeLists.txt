cmake_minimum_required(VERSION 4.1.0-rc1 FATAL_ERROR)

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
set(CMAKE_CXX_MODULE_STD ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS ON)

add_library(asar)
add_library(rapidjson)
add_library(meowmeow)

target_sources(asar
    PUBLIC
    FILE_SET CXX_MODULES FILES
        asar/asar.ixx
)

target_sources(rapidjson
    PUBLIC
    FILE_SET CXX_MODULES FILES
        rapidjson/rapidjson.ixx
)

target_sources(meowmeow
    PUBLIC
    FILE_SET CXX_MODULES FILES
        meowmeow/meowmeow.ixx
)
target_link_libraries(meowmeow
    PRIVATE asar
    PRIVATE rapidjson
)

