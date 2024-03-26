function(msg_add_subdirectory DIRECTORY)
    message(STATUS "Building Ame's ${DIRECTORY}")
    add_subdirectory(${DIRECTORY})
endfunction()

#

msg_add_subdirectory(Core)
msg_add_subdirectory(Engine)

if (AME_BUILD_TESTS)
	function (msg_add_test DIRECTORY_NAME TARGET_NAME)
		message(STATUS "Building test ${DIRECTORY_NAME}_${TARGET_NAME}")
		add_executable("Test_${DIRECTORY_NAME}_${TARGET_NAME}" ${ARGN})
		# directory must contains a library named Ame${DIRECTORY_NAME}, ie AmeCore for Core
		target_link_libraries("Test_${DIRECTORY_NAME}_${TARGET_NAME}" PRIVATE 
			Boost::unit_test_framework
			"Ame::${DIRECTORY_NAME}")
		add_test(NAME "Test_${DIRECTORY_NAME}_${TARGET_NAME}" COMMAND "Test_${DIRECTORY_NAME}_${TARGET_NAME}")
	endfunction()

	function (msg_add_test_directory DIRECTORY_NAME)
		# iterate over all directories inside DIRECTORY_NAME/Tests if they exists
		file(GLOB children RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/${DIRECTORY_NAME}/Tests ${DIRECTORY_NAME}/Tests/*)
		foreach(child ${children})
			if (IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${DIRECTORY_NAME}/Tests/${child})
				# iterate over all files inside the directory
				file(GLOB files ${CMAKE_CURRENT_SOURCE_DIR}/${DIRECTORY_NAME}/Tests/${child}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/${DIRECTORY_NAME}/Tests/${child}/*.hpp)
				msg_add_test(${DIRECTORY_NAME} ${child} ${files})
			endif()
		endforeach()
	endfunction()

    # For each directory, if it has a Tests subdirectory, add it
    file(GLOB children RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *)
    foreach(child ${children})
		if (IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${child})
			if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${child}/Tests)
				msg_add_test_directory(${child})
			endif()
		endif()
	endforeach()
endif()
