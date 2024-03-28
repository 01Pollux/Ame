file(GLOB_RECURSE PRIVATE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/Private/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Private/*.hpp)
file(GLOB_RECURSE PUBLIC_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/Public/*.hpp)

ame_add_library(Rhi STATIC ${PRIVATE_FILES} ${PUBLIC_FILES})
ame_add_pch(Rhi)

# add include directories
target_include_directories(
        Ame_Rhi PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
        ${CMAKE_SOURCE_DIR}/Deps/Externals/NRI/Include)

# link dependencies
target_link_libraries(Ame_Rhi PUBLIC 
    freeimage::FreeImage
    freeimage::FreeImagePlus
    DxCompiler::DxCompiler
    NRI
    Ame::Windowing)
