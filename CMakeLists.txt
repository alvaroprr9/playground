cmake_minimum_required(VERSION 3.16...3.31)
project(playground-root)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(
  onyx
  GIT_REPOSITORY https://github.com/ismawno/onyx.git
  GIT_TAG v0.8.x
  GIT_SHALLOW TRUE
  GIT_PROGRESS TRUE)
FetchContent_MakeAvailable(onyx)

set(CMAKE_BUILD_TYPE
    "Debug"
    CACHE STRING "")

set(SOURCES main.cpp)

add_executable(playground ${SOURCES})

target_link_libraries(playground PRIVATE onyx)
