# CMake list for Leges Motus

cmake_minimum_required(VERSION 2.6)
project(LEGESMOTUS)

set(VERSION 0.4.0-svn)
set(PREFIX /usr/local)
set(DATA_DIR ${PREFIX}/share/games/legesmotus)
include(config.cmake OPTIONAL)

set(FLAG_NOBUNDLE false)
set(FLAG_OPT_USR /opt/local)

set(CMAKE_PREFIX_PATH ${FLAG_OPT_USR})

if(CMAKE_HOST_APPLE)
	if(FLAG_NOBUNDLE)
		execute_process(COMMAND lipo ${FLAG_OPT_USR}/lib/libSDL.dylib -verify_arch x86_64 RESULT_VARIABLE IS_X86)
		execute_process(COMMAND lipo ${FLAG_OPT_USR}/lib/libSDL.dylib -verify_arch i386 RESULT_VARIABLE IS_X64)
		execute_process(COMMAND lipo ${FLAG_OPT_USR}/lib/libSDL.dylib -verify_arch ppc RESULT_VARIABLE IS_PPC)
		if(NOT FLAG_ARCH)
			if(IS_X64)
				set(FLAG_ARCH x86_64)
			elseif(IS_X86)
				set(FLAG_ARCH i386)
			elseif(IS_PPC)
				set(FLAG_ARCH ppc)
			else()
				message(ERROR "Can't autodetect architecture")
			endif()
		endif(NOT FLAG_ARCH)
		set(CMAKE_FIND_FRAMEWORK LAST)
	else(FLAG_NOBUNDLE)
		if(NOT FLAG_ARCH)
			set(FLAG_ARCH ${CMAKE_OSX_ARCHITECTURES})
		endif(NOT FLAG_ARCH)

		add_definitions(-DLM_FWBASED)
		set(CMAKE_FIND_FRAMEWORK FIRST)

		set(CLIENT_DIR ${PROJECT_SOURCE_DIR}/client)
		find_file(DATA_SRC data HINT ${PROJECT_SOURCE_DIR}/..)
	endif(FLAG_NOBUNDLE)

	set(CXXFLAGS "${CXXFLAGS} -arch ${FLAG_ARCH}")
	set(LDFLAGS "${LDFLAGS} -arch ${FLAG_ARCH}")
endif(CMAKE_HOST_APPLE)

add_definitions(-DLM_DATA_DIR="${DATA_DIR}" -DLM_VERSION="${VERSION}")

if(FLAG_NO_UPGRADE_NAG)
	add_definitions(-DLM_NO_UPGRADE_NAG)
endif(FLAG_NO_UPGRADE_NAG)

include_directories(.)

add_subdirectory(common)
add_subdirectory(client)

add_custom_target(common DEPENDS lmcommon)
add_custom_target(client DEPENDS legesmotus)
