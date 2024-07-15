add_engine_submodule(
    TARGET "AmeCore"
    PATH "${CMAKE_CURRENT_SOURCE_DIR}/Core"
    TYPE "static"
    ALIAS "Ame::Core")
    

target_precompile_headers(AmeCore PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Core/Public/Core/Ame.hpp")
target_link_libraries(AmeCore PUBLIC ${Boost_LIBRARIES})
target_link_libraries(AmeCore PUBLIC magic_enum::magic_enum)
target_link_libraries(AmeCore PUBLIC spdlog::spdlog_header_only)
target_link_libraries(AmeCore PUBLIC concurrencpp::concurrencpp)
target_link_libraries(AmeCore PUBLIC potto)
target_link_libraries(AmeCore PUBLIC mimalloc)

#

add_engine_submodule(
    TARGET "AmeWindow"
    PATH "${CMAKE_CURRENT_SOURCE_DIR}/Window"
    TYPE "static"
    ALIAS "Ame::Window")

target_link_libraries(AmeWindow PUBLIC Ame::Core)
target_link_libraries(AmeWindow PUBLIC glfw)

#

add_engine_submodule(
    TARGET "AmeEngine"
    PATH "${CMAKE_CURRENT_SOURCE_DIR}/Engine"
    TYPE "shared"
    ALIAS "Ame::Engine")

target_link_libraries(AmeEngine PUBLIC Ame::Window)

#

install(TARGETS
    AmeCore
    AmeWindow
    AmeEngine)

#

if (${AME_BUILD_EXAMPLES})
    add_subdirectory(Examples)
endif ()
