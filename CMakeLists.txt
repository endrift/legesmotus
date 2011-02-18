cmake_minimum_required(VERSION 2.6)
project("Leges Motus")

set(LM_DATA_DIR "data" CACHE STRING "The data directory")
set(LM_CONFIG_DIR "config" CACHE STRING "The configuration directory")

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
add_subdirectory(client)
add_subdirectory(gui)
add_subdirectory(ai)
add_subdirectory(server)
#add_subdirectory(metaserver)
#add_subdirectory(serverscanner)
