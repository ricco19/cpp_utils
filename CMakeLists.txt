# Project Setup
cmake_minimum_required(VERSION 3.5)
project(cpp_utils)
set(META_PROJECT_NAME "${PROJECT_NAME}")
set(META_PROJECT_DESCRIPTION "Some common C++ utilitys that I use")
set(META_AUTHOR_ORGANIZATION "")
set(META_AUTHOR_DOMAIN "")
set(META_AUTHOR_MAINTAINER "")
set(META_VERSION_MAJOR "1")
set(META_VERSION_MINOR "0")
set(META_VERSION_PATCH "0")
set(META_VERSION_REVISION "${GIT_REV}")
set(META_VERSION "${META_VERSION_MAJOR}.${META_VERSION_MINOR}.${META_VERSION_PATCH}")
set(META_NAME_VERSION "${META_PROJECT_NAME} v${META_VERSION} (${META_VERSION_REVISION})")
set(META_CMAKE_INIT_SHA "${GIT_REV}")
set(CMAKE_VERBOSE_MAKEFILE 0)

# CMake module paths
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

# Output paths
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")

# Sets default build to RelWithDebInfo on single generators
if (NOT GENERATOR_IS_MULTI_CONFIG AND NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message("   -> Setting build type to 'RelWithDebInfo' as none was specified.")
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
        "MinSizeRel" "RelWithDebInfo")
endif()

# Create our executable(s)
add_executable(${PROJECT_NAME}
    src/tests.cpp
)

# If available, add sanitizers to our debug build
if (NOT GENERATOR_IS_MULTI_CONFIG AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(SANITIZE_ADDRESS On)
    set(SANITIZE_UNDEFINED On)
    set(ENV{ASAN_OPTIONS}, "detect_leaks=1")
    find_package(Sanitizers)
    add_sanitizers(${PROJECT_NAME})
endif()

# Includes
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
        $<$<BOOL:${WIN32}>:C:/mingw64/include>
)

# Link external librarys
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        benchmark
        tiff
        turbojpeg
        $<$<BOOL:${WIN32}>:shlwapi>
        $<$<BOOL:${UNIX}>:pthread>
)

# C++17
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED OFF
    CXX_EXTENSIONS OFF
)

# Disable min() and max() macros on windows that conflict with the standard
target_compile_definitions(${PROJECT_NAME} PRIVATE NOMINMAX)

# Load CXX Flags module
include(cxx_flags_setup)

# Install?
