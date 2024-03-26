function(msg_add_subdirectory NAME)
    message(STATUS "Building Sample: ${NAME}")
    add_subdirectory(${NAME})
endfunction()

msg_add_subdirectory(SimpleLog)
