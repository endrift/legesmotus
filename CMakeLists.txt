cmake_minimum_required(VERSION 2.6)
project("Leges Motus")

set(LM_DATA_DIR "data" CACHE STRING "The data directory")
set(LM_CONFIG_DIR "config" CACHE STRING "The configuration directory")

if(CMAKE_HOST_APPLE)
option(LM_BUNDLE "Build as an App Bundle" OFF)

if(LM_BUNDLE)
else()
include_directories(/opt/local/include/)
endif()
endif()

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_BINARY_DIR}/common)

link_directories(${PROJECT_BINARY_DIR}/common)
link_directories(${PROJECT_BINARY_DIR}/client)
link_directories(/opt/local/lib/)

add_subdirectory(common)
add_subdirectory(client)
add_subdirectory(gui)
