add_engine_submodule(
    TARGET "AmeCore"
    PATH "${CMAKE_CURRENT_SOURCE_DIR}/Core"
    TYPE "static"
    ALIAS "Ame::Core")

if (${AME_BUILD_EXAMPLES})
    add_subdirectory(Examples)
endif ()
