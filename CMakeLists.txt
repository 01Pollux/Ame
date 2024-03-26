cmake_minimum_required(VERSION 3.26)

# Require CMAKE_TOOLCHAIN_FILE to be set
if (NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    message(FATAL_ERROR "CMAKE_TOOLCHAIN_FILE must be set")
endif ()

include(CMakeDependentOption)

project(ame_project)

#

# Options:
#
# /Rhi
# AME_RENDERER_FORCE_NULL: Force null renderer
# AME_RENDERER_BUILD_DX12: Build with dx12 backend
# AME_RENDERER_BUILD_VULKAN: Build with vulkan backend
#
# /Extras:
# AME_BUILD_TESTS: Build tests
# AME_BUILD_SAMPLES: Build sample apps
#

#if NoLog=TRUE passed to command line, define AME_NO_LOG
if (DEFINED NoLog)
    add_definitions(-DAME_NO_LOG)
endif ()

option(AME_RENDERER_FORCE_NULL "Force null renderer" OFF)
option(AME_RENDERER_BUILD_DX12 "Build with dx12 backend" ON)
option(AME_RENDERER_BUILD_VULKAN "Build with vulkan backend" ON)

option(AME_BUILD_SAMPLES "Build samples" ON)
option(AME_BUILD_TESTS "Build tests" ON)

cmake_dependent_option(AME_RENDERER_BUILD_DX12 "Build with dx12 backend" OFF "NOT AME_RENDERER_FORCE_NULL;WIN32" OFF)
cmake_dependent_option(AME_RENDERER_BUILD_VULKAN "Build with vulkan backend" OFF "NOT AME_RENDERER_FORCE_NULL" OFF)

set(CMAKE_CXX_STANDARD 23)
set(CXX_STANDARD_REQUIRED ON)
set(CXX_EXTENSIONS NO)

set(NRI_ENABLE_D3D11_SUPPORT OFF CACHE BOOL "" FORCE)
if (AME_RENDERER_FORCE_NULL)
    set(NRI_BACKEND_D3D11 OFF CACHE BOOL "" FORCE)
    set(NRI_BACKEND_D3D12 OFF CACHE BOOL "" FORCE)
    set(NRI_BACKEND_VK OFF CACHE BOOL "" FORCE)
    add_definitions(-DAME_RENDERER_FORCE_NULL)
else ()
    if (AME_RENDERER_BUILD_DX12)
        set(NRI_BACKEND_D3D12 ON CACHE BOOL "" FORCE)
        add_definitions(-DAME_RENDERER_BUILD_DX12)
    endif ()
    if (AME_RENDERER_BUILD_VULKAN)
        set(NRI_BACKEND_VK ON CACHE BOOL "" FORCE)
        add_definitions(-DAME_RENDERER_BUILD_VULKAN)
    endif ()
endif ()

if (AME_BUILD_TESTS)
    enable_testing()
endif ()

# Release=Max speed, Dist=Max speed without debug
if (AME_BUILD_TYPE MATCHES Dist)
    # Remove console window on dist build
    if (WIN32)
        set(SUBSYSTEM_LINKER_OPTIONS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
    else()
        set(SUBSYSTEM_LINKER_OPTIONS "-mwindows")
    endif()
    add_definitions(-DAME_DIST)
elseif (AME_BUILD_TYPE MATCHES Release)
    add_definitions(-DAME_RELEASE)
elseif (AME_BUILD_TYPE MATCHES Debug)
    add_definitions(-DAME_DEBUG)
    add_definitions(-DAME_ASSET_MGR_DISABLE_HASH_VALIDATION) # Disable asset hash validation in debug
    if(MSVC)
        add_link_options("/INCREMENTAL")
    endif()
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<$<CONFIG:Debug>:EditAndContinue>")
endif ()

# Add NOMINMAX to disable min/max macros
add_definitions(-DNOMINMAX)

# Detect platform
if (UNIX AND NOT APPLE)
    set(AME_PLATFORM_LINUX ON)
    add_definitions(-DAME_PLATFORM_LINUX)
elseif (WIN32)
    set(AME_PLATFORM_WINDOWS ON)
    add_definitions(-DAME_PLATFORM_WINDOWS)
else ()
    message(FATAL_ERROR "Unsupported platform")
endif ()

# set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${AME_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/${AME_BUILD_TYPE})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/obj/${AME_BUILD_TYPE})

#

# Append Deps/CMakeModules to module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/Deps/Externals/")

# Add Asset directory
set(AME_ASSETS_DIR ${CMAKE_SOURCE_DIR}/Assets)

#

# Helper function to set variable for local and parent scope
macro(set_local_and_parent NAME VALUE)
    set(${NAME} ${VALUE} CACHE INTERNAL "" FORCE)
    set(${NAME} ${VALUE} PARENT_SCOPE)
endmacro()

# Helper function to copy assets to build directory
function(ame_copy_asset_resources TARGET_NAME ASSET_SRC ASSET_DST)
    add_custom_command(TARGET "Ame_${TARGET_NAME}" POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${AME_ASSETS_DIR}/${ASSET_SRC}
            $<TARGET_FILE_DIR:${TARGET}>/${ASSET_DST})
endfunction()

# Helper function to add the C++ library
# ame_add_library(Name Source1 Source2 ...)
function(ame_add_library TARGET_NAME LIBRARY_TYPE)
    add_library("Ame_${TARGET_NAME}" ${LIBRARY_TYPE} ${ARGN})
    add_library("Ame::${TARGET_NAME}" ALIAS "Ame_${TARGET_NAME}")
    set_target_properties("Ame_${TARGET_NAME}" PROPERTIES
            LINKER_LANGUAGE CXX)
endfunction()

# Helper function to add the executable inside CMAKE_RUNTIME_OUTPUT_DIRECTORY/Name
# ame_add_executable(Name Source1 Source2 ...)
function(ame_add_executable TARGET_NAME)
    ame_add_library(${TARGET_NAME} STATIC ${ARGN})
    add_executable(${TARGET_NAME} ${ARGN})
    set_target_properties(${TARGET_NAME} PROPERTIES
			RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET_NAME})
    
    # check if SUBSYSTEM_LINKER_OPTIONS is was set()
    if (DEFINED SUBSYSTEM_LINKER_OPTIONS)
        set_target_properties("Ame_${TARGET_NAME}" PROPERTIES LINK_FLAGS ${SUBSYSTEM_LINKER_OPTIONS})
    endif ()

    ame_copy_dxil_and_dxcompiler(${TARGET_NAME})
    ame_copy_nri_library(${TARGET_NAME})
endfunction()

# Helper function to add pch to target
# ame_add_pch(Name) -> Name_PCH.hpp
function(ame_add_pch TARGET_NAME)
    target_precompile_headers("Ame_${TARGET_NAME}" PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_NAME}_PCH.hpp)
endfunction()

#

add_definitions(-DGLM_ENABLE_EXPERIMENTAL)
add_definitions(-DGLM_FORCE_DEPTH_ZERO_TO_ONE)
add_definitions(-DGLM_FORCE_LEFT_HANDED)

# Add third party libraries
find_package(assimp REQUIRED)
find_package(glm REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(magic_enum REQUIRED)
find_package(Boost COMPONENTS REQUIRED
		program_options
        serialization
        filesystem)
find_package(cryptopp REQUIRED)
find_package(concurrencpp REQUIRED)
find_package(freeimage REQUIRED)
find_package(flecs REQUIRED)
find_package(Bullet REQUIRED)

if (AME_BUILD_TESTS)
	find_package(Boost COMPONENTS REQUIRED unit_test_framework)
endif ()

#

add_subdirectory(Deps/Externals)
add_subdirectory(Ame)

if (AME_BUILD_SAMPLES)
    add_subdirectory(Samples)
endif ()