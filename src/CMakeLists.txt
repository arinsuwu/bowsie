cmake_minimum_required(VERSION 4.0.0 FATAL_ERROR)

project(bowsie VERSION 1.10 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS ON)

add_subdirectory(${SOURCE_DIR}include)

add_executable(bowsie)
target_sources(bowsie
    PUBLIC ${SOURCE_DIR}bowsie.cpp
    PUBLIC ${SOURCE_DIR}include/misc.cpp
    PUBLIC ${SOURCE_DIR}include/map16.cpp
    PUBLIC ${SOURCE_DIR}include/rom.cpp
    PUBLIC ${SOURCE_DIR}include/settings.cpp
    PUBLIC resources/bowsie.rc
)

target_link_libraries(bowsie
    PRIVATE fmt::fmt
    PRIVATE asar-static
)

