add_engine_submodule(TARGET "AmeHelloWorld" PATH " ${CMAKE_CURRENT_SOURCE_DIR}/HelloWorld" TYPE "executable")
target_link_libraries(AmeHelloWorld Ame::Core)
