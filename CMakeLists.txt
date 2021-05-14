cmake_minimum_required(VERSION 3.15)

project(Cafe.Core CXX)

include(${CMAKE_CURRENT_SOURCE_DIR}/CafeCommon/cmake/CafeCommon.cmake)

add_library(Cafe.Core INTERFACE)

target_include_directories(Cafe.Core INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>)

target_compile_features(Cafe.Core INTERFACE cxx_std_20)

AddCafeSharedFlags(Cafe.Core)

# 见 Cafe::Core::Misc::MakeSingleParamTemplate 注释
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    target_compile_options(Cafe.Core INTERFACE -frelaxed-template-template-args)
endif()

install(TARGETS Cafe.Core
    EXPORT Core)

install(DIRECTORY src/
    TYPE INCLUDE
    FILES_MATCHING PATTERN "*.h")

install(EXPORT Core
    DESTINATION cmake/Core)

if(CAFE_INCLUDE_TESTS)
    add_subdirectory(Test)
endif()
