file(GLOB_RECURSE PRIVATE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/Private/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Private/*.hpp)
file(GLOB_RECURSE PUBLIC_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/Public/*.hpp)

ame_add_library(Framework STATIC ${PRIVATE_FILES} ${PUBLIC_FILES})
ame_add_pch(Framework)

# add include directories
target_include_directories(
        Ame_Framework PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Public)

target_link_libraries(
    Ame_Framework PUBLIC 
    Ame::Engine)
