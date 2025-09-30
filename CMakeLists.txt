cmake_minimum_required(VERSION 3.30)
project(interval VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(icl INTERFACE)
add_library(icl::icl ALIAS icl)

target_include_directories(icl INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

option(ICL_BUILD_TEST "Build test" OFF)

if(ICL_BUILD_TEST)
    include(FetchContent)

    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.10.0
    )

    FetchContent_MakeAvailable(Catch2)

    list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

    include(CTest)
    include(Catch)

    add_subdirectory(test)
endif()
