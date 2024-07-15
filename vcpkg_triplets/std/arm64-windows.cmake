set(VCPKG_TARGET_ARCHITECTURE arm64)

set(VCPKG_CRT_LINKAGE dynamic)

if(${PORT} MATCHES "freeimage|bullet3")
	set(VCPKG_LIBRARY_LINKAGE dynamic)
else()
	set(VCPKG_LIBRARY_LINKAGE static)
endif()