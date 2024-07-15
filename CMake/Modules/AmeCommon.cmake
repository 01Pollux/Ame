
include(FetchContent)

if (${AME_DISABLE_LOGGING})
    add_definitions(-DAME_DISABLE_LOGGING)
endif()

# Add NOMINMAX to disable min/max macros
add_definitions(-DNOMINMAX)

# add build options that are "Debug, Release and RelWithDebInfo"
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo")

# if we are debug or release
if (CMAKE_BUILD_TYPE MATCHES "Debug")
    set(AME_BUILD_TYPE "Debug")
    add_definitions(-DAME_DEBUG)
elseif (CMAKE_BUILD_TYPE MATCHES "Release")
    set(AME_BUILD_TYPE "Release")
    add_definitions(-DAME_DIST)
else()
    set(AME_BUILD_TYPE "RelWithDebInfo")
    add_definitions(-DAME_RELEASE)
endif()


# fetch sanitizer url content from internet only if we are in debug or release mode with debug info
if (CMAKE_BUILD_TYPE MATCHES "Debug" OR CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")
    include(FetchContent)
    FetchContent_Declare(
        sanitizer
        GIT_REPOSITORY https://github.com/arsenm/sanitizers-cmake.git
        SOURCE_DIR ${AME_THIRDPARTY_DIR}/sanitizer)
    FetchContent_MakeAvailable(sanitizer)

    # add sanitizer to the project
    set(AME_SANITIZER_ENABLED ON)
else()
    set(AME_SANITIZER_ENABLED OFF)
endif()

if (MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

function (add_engine_submodule)
    set(ONE_VALUE_ARGS TARGET PATH TYPE ALIAS)
    cmake_parse_arguments(ARG "" "${ONE_VALUE_ARGS}" "" ${ARGN})

    file(GLOB_RECURSE PRIVATE_FILES
            ${ARG_PATH}/Private/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/${ARG_PATH}/Private/*.hpp)
    file(GLOB_RECURSE PUBLIC_FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/${ARG_PATH}/Public/*.hpp)
    
    set(ADD_PRIVATE_FILES OFF)
    set(ADD_PUBLIC_TYPE PUBLIC)
    
    # ARG_TYPE can be either "static" or "shared" or "interface" or "executable"
    if ("${ARG_TYPE}" STREQUAL "static")
        add_library(${ARG_TARGET} STATIC ${PRIVATE_FILES} ${PUBLIC_FILES})
        set(ADD_PRIVATE_FILES ON)
    elseif ("${ARG_TYPE}" STREQUAL "shared")
        add_library(${ARG_TARGET} SHARED ${PRIVATE_FILES} ${PUBLIC_FILES})
        set(ADD_PRIVATE_FILES ON)
    elseif ("${ARG_TYPE}" STREQUAL "interface")
        add_library(${ARG_TARGET} INTERFACE)
        set(ADD_PUBLIC_TYPE INTERFACE)
    elseif ("${ARG_TYPE}" STREQUAL "executable")
        add_executable(${ARG_TARGET} ${PRIVATE_FILES} ${PUBLIC_FILES})
        set(ADD_PRIVATE_FILES ON)
    else()
        message(FATAL_ERROR "Invalid SUBMODULE_TYPE: ${SUBMODULE_TYPE}")
    endif()

    target_include_directories(${ARG_TARGET} ${ADD_PUBLIC_TYPE} ${CMAKE_CURRENT_SOURCE_DIR}/${ARG_PATH}/Public)
    if (${ADD_PRIVATE_FILES})
        target_include_directories(${ARG_TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/${ARG_PATH}/Private)
    endif()

    if (ARG_ALIAS)
        add_library(${ARG_ALIAS} ALIAS ${ARG_TARGET})
    endif()
endfunction()
