file(GLOB_RECURSE PRIVATE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/Private/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Private/*.hpp)
file(GLOB_RECURSE PUBLIC_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/Public/*.hpp)

ame_add_library(Core STATIC ${PRIVATE_FILES} ${PUBLIC_FILES})
ame_add_pch(Core)

# add include directories
target_include_directories(
        Ame_Core PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
        ${BEXT_DI_INCLUDE_DIRS})

# link dependencies
target_link_libraries(Ame_Core PUBLIC fmt::fmt-header-only)
target_link_libraries(Ame_Core PUBLIC spdlog::spdlog_header_only)
target_link_libraries(Ame_Core PUBLIC glm::glm)
target_link_libraries(Ame_Core PUBLIC magic_enum::magic_enum)
target_link_libraries(Ame_Core PUBLIC ${Boost_LIBRARIES})
target_link_libraries(Ame_Core PUBLIC cryptopp::cryptopp)
target_link_libraries(Ame_Core PUBLIC concurrencpp::concurrencpp)
target_link_libraries(Ame_Core PUBLIC flecs::flecs_static)
