file(GLOB_RECURSE PRIVATE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/Private/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Private/*.hpp)
file(GLOB_RECURSE PUBLIC_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/Public/*.hpp)

ame_add_library(AmeCore STATIC ${PRIVATE_FILES} ${PUBLIC_FILES})
ame_add_pch(AmeCore)

# add include directories
target_include_directories(
        AmeCore PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
        ${BEXT_DI_INCLUDE_DIRS})

# link dependencies
target_link_libraries(AmeCore PUBLIC fmt::fmt-header-only)
target_link_libraries(AmeCore PUBLIC spdlog::spdlog_header_only)
target_link_libraries(AmeCore PUBLIC glm::glm)
target_link_libraries(AmeCore PUBLIC magic_enum::magic_enum)
target_link_libraries(AmeCore PUBLIC ${Boost_LIBRARIES})
target_link_libraries(AmeCore PUBLIC cryptopp::cryptopp)
target_link_libraries(AmeCore PUBLIC concurrencpp::concurrencpp)
