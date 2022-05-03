set(SVENMOD_LAUNCHER "svenmod_launcher")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (MSVC)
	set(SVENMOD_LAUNCHER_SOURCE_FILES
		${CMAKE_SOURCE_DIR}/launcher/launcher.cpp
		
		${CMAKE_SOURCE_DIR}/shared/commandline.cpp
		${CMAKE_SOURCE_DIR}/shared/interface.cpp
		${CMAKE_SOURCE_DIR}/shared/registry.cpp
		${CMAKE_SOURCE_DIR}/shared/sys.cpp
	)
else()
	set(SVENMOD_LAUNCHER_SOURCE_FILES
		${CMAKE_SOURCE_DIR}/launcher/launcher.cpp
		
		${CMAKE_SOURCE_DIR}/shared/commandline.cpp
		${CMAKE_SOURCE_DIR}/shared/interface.cpp
		${CMAKE_SOURCE_DIR}/shared/sys.cpp
	)
endif()

if (MSVC)
	add_executable(${SVENMOD_LAUNCHER} WIN32 ${SVENMOD_LAUNCHER_SOURCE_FILES})
else()
	add_executable(${SVENMOD_LAUNCHER} ${SVENMOD_LAUNCHER_SOURCE_FILES})
endif()

add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE)

target_compile_definitions(${SVENMOD_LAUNCHER} PRIVATE
	STATIC_LINKED
)

if (MSVC)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT /DNDEBUG")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
else()
	set(CMAKE_CXX_FLAGS "-m32 -g -std=c++14 -w -Wl,--no-as-needed -ldl")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3 -s -DNDEBUG")
	set(CMAKE_CXX_FLAGS_DEBUG "-Os")
endif()
