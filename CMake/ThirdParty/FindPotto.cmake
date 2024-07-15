set(POTTO_BUILD_DEMO OFF)
add_subdirectory(${CMAKE_SOURCE_DIR}/ThirdParty/potto)

target_include_directories(potto INTERFACE ${CMAKE_SOURCE_DIR}/ThirdParty/potto/include)
install(TARGETS potto)