cmake_minimum_required(VERSION 4.0.0 FATAL_ERROR)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS ON)

set(ASAR_TARGET "asar-static")

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG        40626af88bd7df9a5fb80be7b25ac85b122d6c21
)
FetchContent_MakeAvailable(fmt)
  
FetchContent_Declare(
  asar
  GIT_REPOSITORY https://github.com/RPGHacker/asar
  GIT_TAG v1.91
  SOURCE_SUBDIR src
)
FetchContent_MakeAvailable(asar)

set_target_properties(${ASAR_TARGET} PROPERTIES CXX_STANDARD 14)
    
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  set_target_properties(${ASAR_TARGET} PROPERTIES COMPILE_OPTIONS "-w;-fPIC")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set_target_properties(${ASAR_TARGET} PROPERTIES COMPILE_OPTIONS "/w")
else ()
  message(FATAL_ERROR "Unrecognized compiler")
endif()

