if (AME_SANITIZER_ENABLED)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/ThirdParty/sanitizers-cmake/cmake")
    
    add_definitions(-D_DISABLE_VECTOR_ANNOTATION)
    add_definitions(-D_DISABLE_STRING_ANNOTATION)
    set(SANITIZE_ADDRESS TRUE)
    set(SANITIZE_UNDEFINED TRUE)
    find_package(Sanitizers REQUIRED)
endif ()

find_package(Boost REQUIRED COMPONENTS
    program_options
    serialization
    filesystem)
find_package(concurrencpp CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(mimalloc CONFIG REQUIRED)
find_package(Potto REQUIRED)

if (AME_SANITIZER_ENABLED)
    add_sanitizers(potto)
endif ()