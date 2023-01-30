set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_library("common" STATIC features.cpp global_ifaces.cpp sys.cpp messagebuffer.cpp)
add_library("convar" STATIC convar.cpp)
add_library("interface" STATIC interface.cpp)

add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE)

if (MSVC)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD /DNDEBUG")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
else()
	set(CMAKE_CXX_FLAGS "-m32 -g -std=c++14 -w -Wl,--no-as-needed -ldl")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3 -s -DNDEBUG")
	set(CMAKE_CXX_FLAGS_DEBUG "-Os")
endif()