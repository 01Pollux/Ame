add_engine_submodule(TARGET "Example_HelloWorld" PATH "${CMAKE_CURRENT_SOURCE_DIR}/HelloWorld" TYPE "executable")
target_link_libraries(Example_HelloWorld Ame::Core)

add_engine_submodule(TARGET "Example_SimpleWindow" PATH "${CMAKE_CURRENT_SOURCE_DIR}/SimpleWindow" TYPE "executable")
target_link_libraries(Example_SimpleWindow Ame::Engine)
configure_copy_engine_dlls(TARGET Example_SimpleWindow)

install(TARGETS
		Example_HelloWorld
		Example_SimpleWindow)
