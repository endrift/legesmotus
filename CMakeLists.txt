cmake_minimum_required(VERSION 2.6)
project("Leges Motus")

set(LM_DATA_DIR "data" CACHE STRING "The data directory")
set(LM_CONFIG_DIR "config" CACHE STRING "The configuration directory")

option(LM_BUILD_GUI "Build the Leges Motus graphical client" ON)
option(LM_BUILD_AI "Build the Leges Motus AIs" ON)
option(LM_BUILD_SERVER "Build the Leges Motus server" ON)
option(LM_BUILD_METASERVER "Build the Leges Motus metaserver (advanced)" OFF)
option(LM_BUILD_SERVERSCANNER "Build the Leges Motus server scanner" OFF)

if(CMAKE_HOST_APPLE)
option(LM_BUNDLE "Build as a Mac OS X Application Bundle" OFF)
option(LM_UNIVERSAL "Build as a Mac OS X Universal Binary" OFF)

if(NOT LM_BUNDLE)
set(CMAKE_FIND_FRAMEWORK LAST)
endif()
add_definitions(-D LM_FWBASED)
set(CMAKE_FIND_FRAMEWORK FIRST)
endif()

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_BINARY_DIR}/common)

link_directories(${PROJECT_BINARY_DIR}/common)
link_directories(${PROJECT_BINARY_DIR}/client)

# Find Box2d
if(NOT BOX2D_INCLUDE_DIR)
find_path(BOX2D_INCLUDE_DIR Box2D/Box2D.h
	HINTS
	$ENV{BOX2D_DIR}
	PATH_SUFFIXES include
	PATHS
	~/Library/Frameworks
	/Library/Frameworks
	/usr/local
	/usr
	/sw
	/opt/local
	/opt
)
endif()

if(NOT BOX2D_LIBRARY)
find_library(BOX2D_LIBRARY Box2D
	HINTS
	$ENV{BOX2D_DIR}
	PATH_SUFFIXES lib lib64 libexec
	PATHS
	~/Library/Frameworks
	/Library/Frameworks
	/usr/local
	/usr
	/sw
	/opt/local
	/opt
)
endif()

if(BOX2D_INCLUDE_DIR AND BOX2D_LIBRARY)
message(STATUS "Found Box2D: ${BOX2D_LIBRARY}")
include_directories(${BOX2D_INCLUDE_DIR})
set(LIBS ${LIBS} ${BOX2D_LIBRARY})
else()
message(FATAL_ERROR "Could not find Box2D")
endif()

add_subdirectory(common)

if(LM_BUILD_GUI OR LM_BUILD_AI)
add_subdirectory(client)
endif()

if(LM_BUILD_GUI)
add_subdirectory(gui)
endif()

if(LM_BUILD_AI)
add_subdirectory(ai)
endif()

if(LM_BUILD_SERVER)
add_subdirectory(server)
endif()

if(LM_BUILD_METASERVER)
add_subdirectory(metaserver)
endif()

if(LM_BUILD_SERVERSCANNER)
add_subdirectory(serverscanner)
endif()
