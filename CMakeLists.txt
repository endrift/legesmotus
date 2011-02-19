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

#if(NOT LM_BUNDLE)
set(CMAKE_FIND_FRAMEWORK LAST)
include_directories(/opt/local/include/)
#endif()
endif()

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_BINARY_DIR}/common)

link_directories(${PROJECT_BINARY_DIR}/common)
link_directories(${PROJECT_BINARY_DIR}/client)
link_directories(/opt/local/lib/)

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
