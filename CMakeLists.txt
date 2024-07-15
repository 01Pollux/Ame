if (CMAKE_BUILD_TYPE MATCHES "Debug" OR CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")
    # add sanitizer to the project
    set(AME_SANITIZER_ENABLED ON)
else()
    set(AME_SANITIZER_ENABLED OFF)
endif()

#

if (AME_SANITIZER_ENABLED)
    set(VCPKG_OVERLAY_TRIPLETS "${CMAKE_SOURCE_DIR}/vcpkg_triplets/asan" CACHE STRING "Vcpkg overlay triplets path")
else ()
    set(VCPKG_OVERLAY_TRIPLETS "${CMAKE_SOURCE_DIR}/vcpkg_triplets/std" CACHE STRING "Vcpkg overlay triplets path")
endif ()

#

cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 23)
set(CXX_STANDARD_REQUIRED ON)
set(CXX_EXTENSIONS NO)

project(Ame2)

# Require CMAKE_TOOLCHAIN_FILE to be set
if (NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
endif ()

set(DST_BASE_INCLUDE_DIR include)
set(DST_INCLUDE_DIR ${DEST_BASE_INCLUDE_DIR}/Ame2)
set(DST_BIN_DIR bin)
set(DST_LIB_DIR lib)
set(DST_ARCH_DIR obj)
set(DST_SHARE_DIR share)
set(DST_THIRDPARTY_INC_DIR ${DST_INCLUDE_DIR}/thirdparty)

set(AME_SRC_DIR ${CMAKE_SOURCE_DIR}/Source)
set(AME_THIRDPARTY_DIR ${CMAKE_SOURCE_DIR}/ThirdParty)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake/Modules")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake/ThirdParty")
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

include(ucm)
include(AmeOptions)
include(AmeCommon)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${DST_ARCH_DIR}/${AME_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${DST_LIB_DIR}/${AME_BUILD_TYPE})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${DST_BIN_DIR}/${AME_BUILD_TYPE})

add_subdirectory(./Source)