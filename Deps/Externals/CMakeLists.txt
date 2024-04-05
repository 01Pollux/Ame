include(GNUInstallDirs)

function(msg_add_subdirectory DIRECTORY)
    message(STATUS "Building ${DIRECTORY}")
    add_subdirectory(${DIRECTORY})
endfunction()

function(msg_include_module NAME)
    message(STATUS "Building ${NAME}")
    include("${NAME}.cmake")
endfunction()

function (configure_for_xmake NAME)
    install(TARGETS NAME
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
endfunction()

#

# Add glfw
msg_add_subdirectory(glfw)
configure_for_xmake(GLFW)

# Add imgui
msg_add_subdirectory(DxC)
configure_for_xmake(DxCompiler)

# Add nri
msg_add_subdirectory(nri)
configure_for_xmake(NRI)

# Add nri
msg_include_module(imgui)
configure_for_xmake(imgui)

#

# Override nri's output directory
set_target_properties(NRI PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/NRI"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/NRI"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/NRI")
    
# create global function to copy nri output bin to target
function(ame_copy_nri_library TARGET_NAME)
    message(STATUS "Copying NRI files to ${TARGET_NAME}")
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/NRI"
		$<TARGET_FILE_DIR:${TARGET_NAME}>)
endfunction()
set_local_and_parent(ame_copy_nri_library ame_copy_nri_library)

#

# create global function to copy dxil and dxcompiler to target
function(ame_copy_dxil_and_dxcompiler target)
    message(STATUS "Copying ${DXCOMPILER_DXIL_PATH} and ${DXCOMPILER_COMPILER_PATH} to ${target}")
	add_custom_command(TARGET ${target} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"${DXCOMPILER_DXIL_PATH}" $<TARGET_FILE_DIR:${target}>
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"${DXCOMPILER_COMPILER_PATH}" $<TARGET_FILE_DIR:${target}>
	)
endfunction()
set_local_and_parent(ame_copy_dxil_and_dxcompiler ame_copy_dxil_and_dxcompiler)
