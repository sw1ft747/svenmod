cmake_minimum_required(VERSION 3.3.2)

project(SvenMod)

set(SVENMOD_DLL "svenmod")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(SVENMOD_SOURCE_FILES
	svenmod/client.cpp
	svenmod/client_weapon.cpp
	svenmod/config_manager.cpp
	svenmod/console.cpp
	svenmod/cvar.cpp
	svenmod/game_hooks.cpp
	svenmod/render.cpp
	svenmod/detours_api.cpp
	svenmod/gamedata_finder.cpp
	svenmod/gameui_iface.cpp
	svenmod/hooks.cpp
	svenmod/inventory.cpp
	svenmod/keyvalues_custom_parser.cpp
	svenmod/logging_system.cpp
	svenmod/memory_utils.cpp
	svenmod/patterns.cpp
	svenmod/player_utils.cpp
	svenmod/plugins_manager.cpp
	svenmod/svenmod.cpp
	svenmod/svenmod_api.cpp
	svenmod/utils.cpp
	svenmod/vgui_iface.cpp
	
	shared/convar.cpp
	shared/interface.cpp
	shared/mathlib.cpp
	shared/sys.cpp
	shared/messagebuffer.cpp
	
	public/udis86/src/decode.c
	public/udis86/src/itab.c
	public/udis86/src/syn.c
	public/udis86/src/syn-att.c
	public/udis86/src/syn-intel.c
	public/udis86/src/udis86.c
)

include_directories(public)
include_directories(svenmod)
include_directories(launcher)
include_directories(shared)

add_subdirectory(shared)
add_subdirectory(launcher)

add_library(${SVENMOD_DLL} SHARED ${SVENMOD_SOURCE_FILES})

target_link_libraries(${SVENMOD_DLL} OpenGL32)

add_definitions(-DSC_NEWEST)
add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE)

target_compile_definitions(${SVENMOD_DLL} PRIVATE
	DBG_ASSERT
	DBG_ASSERT_FATAL
	PLATFORM_EXPORTS
	LIBUDIS86_EXPORTS
	SYS_UTILS_DYNAMIC_LINKED
	SYS_UTILS_EXPORTS
)

if (MSVC)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT /DNDEBUG")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
else()
	set(CMAKE_CXX_FLAGS "-m32 -g -std=c++14 -w -Wl,--no-as-needed -ldl, -Wl,-soname,${SVENMOD_DLL}.so")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3 -s -DNDEBUG")
	set(CMAKE_CXX_FLAGS_DEBUG "-Os")
endif()
