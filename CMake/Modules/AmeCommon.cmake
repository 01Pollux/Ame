
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

# Including packages
include(AmePackages)

if (MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

function (add_engine_submodule)
    set(ONE_VALUE_ARGS TARGET PATH TYPE ALIAS)
    cmake_parse_arguments(ARG "" "${ONE_VALUE_ARGS}" "" ${ARGN})

    set(PRIVATE_FILES "")
    set(PUBLIC_FILES "")

    file(GLOB_RECURSE PRIVATE_FILES
            "${ARG_PATH}/Private/*.cpp"
            "${ARG_PATH}/Private/*.hpp")
    file(GLOB_RECURSE PUBLIC_FILES
            "${ARG_PATH}/Public/*.hpp")
    
    set(IS_INTERFACE OFF)
    set(ADD_PUBLIC_TYPE PUBLIC)
    
    # ARG_TYPE can be either "static" or "shared" or "interface" or "executable"
    if ("${ARG_TYPE}" STREQUAL "static")
        add_library(${ARG_TARGET} STATIC ${PRIVATE_FILES} ${PUBLIC_FILES})
    elseif ("${ARG_TYPE}" STREQUAL "shared")
        add_library(${ARG_TARGET} SHARED ${PRIVATE_FILES} ${PUBLIC_FILES})
    elseif ("${ARG_TYPE}" STREQUAL "interface")
        add_library(${ARG_TARGET} INTERFACE)
        set(IS_INTERFACE ON)
        set(ADD_PUBLIC_TYPE INTERFACE)
    elseif ("${ARG_TYPE}" STREQUAL "executable")
        add_executable(${ARG_TARGET} ${PRIVATE_FILES} ${PUBLIC_FILES})
    else()
        message(FATAL_ERROR "Invalid SUBMODULE_TYPE: ${SUBMODULE_TYPE}")
    endif()

    target_include_directories(${ARG_TARGET} ${ADD_PUBLIC_TYPE} "${ARG_PATH}/Public")
    if (NOT ${IS_INTERFACE})
        if (${AME_SANITIZER_ENABLED})
            add_sanitizers(${ARG_TARGET})
        endif()
        target_include_directories(${ARG_TARGET} PRIVATE "${ARG_PATH}/Private")
    endif()

    if (ARG_ALIAS)
        add_library(${ARG_ALIAS} ALIAS ${ARG_TARGET})
    endif()
endfunction()

function (link_whole_archive)
	set(ONE_VALUE_ARGS TARGET)
    set(MULTI_VALUE_ARGS LIBS)
	cmake_parse_arguments(ARG "" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}" ${ARGN})

    foreach (LIB ${ARG_LIBS})
	    if (MSVC)
		    target_link_options(${ARG_TARGET} PRIVATE "/WHOLEARCHIVE:$<TARGET_FILE_NAME:${LIB}>")
	    else()
		    target_link_libraries(${ARG_TARGET} PRIVATE "-Wl,--whole-archive" ${LIB} "-Wl,--no-whole-archive")
	    endif()
    endforeach ()
endfunction()

function (configure_module_library)
    set(ONE_VALUE_ARGS TARGET)
    cmake_parse_arguments(ARG "" "${ONE_VALUE_ARGS}" "" ${ARGN})

    add_custom_command(TARGET ${ARG_TARGET} POST_BUILD
        COMMAND "$<TARGET_FILE:pt-gen>" -modulelib
        "$<TARGET_FILE_DIR:${ARG_TARGET}>"
        "$<TARGET_FILE_DIR:${ARG_TARGET}>/module.aml"

        DEPENDS
        "$<TARGET_FILE:pt-gen>"

        VERBATIM
    )
endfunction ()

function (configure_copy_engine_dlls)
    set(ONE_VALUE_ARGS TARGET)
    cmake_parse_arguments(ARG "" "${ONE_VALUE_ARGS}" "" ${ARGN})

    configure_module_library(TARGET ${ARG_TARGET})

    add_custom_command(TARGET ${ARG_TARGET} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:AmeEngine>"
        "$<TARGET_FILE_DIR:${ARG_TARGET}>"
    
        DEPENDS
        "$<TARGET_FILE:AmeEngine>"
        
        VERBATIM
    )

    add_custom_command(TARGET ${ARG_TARGET} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:potto>"
        "$<TARGET_FILE_DIR:${ARG_TARGET}>"
    
        DEPENDS
        "$<TARGET_FILE:potto>"
        
        VERBATIM
    )
endfunction ()