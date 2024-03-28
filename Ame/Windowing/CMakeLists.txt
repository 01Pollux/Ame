file(GLOB_RECURSE PRIVATE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/Private/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Private/*.hpp)
file(GLOB_RECURSE PUBLIC_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/Public/*.hpp)

ame_add_library(Windowing STATIC ${PRIVATE_FILES} ${PUBLIC_FILES})
ame_add_pch(Windowing)

# add include directories
target_include_directories(
        Ame_Windowing PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Public)

# link dependencies
target_link_libraries(Ame_Windowing PUBLIC Ame::Core)
target_link_libraries(Ame_Windowing PUBLIC glfw)
