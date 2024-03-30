file(GLOB_RECURSE PRIVATE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE PUBLIC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)

ame_add_executable(Sample_MultiRhi_MultiEngine ${PRIVATE_FILES} ${PUBLIC_FILES})

target_link_libraries(Sample_MultiRhi_MultiEngine PRIVATE Ame::Engine)
