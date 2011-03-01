# Don't use pkg-config if we're building a bundle
if(NOT CMAKE_HOST_APPLE OR NOT LM_BUNDLE)
find_package(PkgConfig)
endif()

if(PKG_CONFIG_FOUND)

# Stupid hacks for splitting on semicolons
macro(semitospace VAR VALUES)
	set(${VAR} "")
	foreach(VAL ${VALUES})
		set(${VAR} "${${VAR}} ${VAL}")
	endforeach()
endmacro()

macro(addldflags VAR)
	semitospace(_S "${VAR}")
	set(LDFLAGS "${LDFLAGS} ${_S}")
endmacro()

macro(addcflags VAR)
	semitospace(_S "${VAR}")
	set(CFLAGS "${CFLAGS} ${_S}")
endmacro()

macro(find_libraries LIBNAME LIBDEPS)
	pkg_check_modules(${LIBNAME} REQUIRED ${LIBDEPS})

	include_directories(${${LIBNAME}_INCLUDE_DIRS})
	set(LIBS ${LIBS} ${${LIBNAME}_LIBRARIES})

	addldflags("${${LIBNAME}_LDFLAGS_OTHER}")
	addcflags("${${LIBNAME}_CFLAGS_OTHER}")
endmacro()

message(STATUS "Using pkg-config to locate dependencies")

find_libraries(SDL sdl>=1.2)
find_libraries(SDL_IMAGE SDL_image>=1.2)
find_libraries(FREETYPE freetype2)

else()

find_package(SDL REQUIRED)
include_directories(${SDL_INCLUDE_DIR})
set(LIBS ${LIBS} ${SDLMAIN_LIBRARY} ${SDL_LIBRARY})

find_package(SDL_image REQUIRED)
include_directories(${SDLIMAGE_INCLUDE_DIR})
set(LIBS ${LIBS} ${SDLIMAGE_LIBRARY})

find_package(Freetype REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIR_ft2build} ${FREETYPE_INCLUDE_DIR_freetype2})
set(LIBS ${LIBS} ${FREETYPE_LIBRARIES})

endif()

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
set(LIBS ${LIBS} ${OPENGL_LIBRARIES})

add_library(lmgui
	Widget.cpp
	DrawContext.cpp
	GLESContext.cpp
	GuiWindow.cpp
	Font.cpp
	Label.cpp
	Image.cpp
	ResourceCache.cpp
	GraphicContainer.cpp
	Graphic.cpp
	Sprite.cpp
	Bone.cpp
	GraphicRegion.cpp
	Window.cpp
	SDLWindow.cpp
	GuiClient.cpp
	GraphicalMap.cpp
	GraphicalPlayer.cpp
	InputDriver.cpp
	SDLInputDriver.cpp
	InputSink.cpp
	HumanController.cpp
	GameView.cpp
	input.cpp
	GraphicalMapObject.cpp
	ShaderSet.cpp
	GLESProgram.cpp
	Bindings.cpp
	PhysicsDraw.cpp
	GraphicalGate.cpp
	GraphicalWeapon.cpp
	ConvolveKernel.cpp
	Hud.cpp
	pubsub.cpp
	ProgressBar.cpp
)

add_executable(legesmotus WIN32 main.cpp)

if(CMAKE_HOST_APPLE)
if(LM_BUNDLE)
set_target_properties(legesmotus PROPERTIES MACOSX_BUNDLE ON)
set_target_properties(legesmotus PROPERTIES MACOSX_BUNDLE_ICON_FILE legesmotus.icns)
set_target_properties(legesmotus PROPERTIES MACOSX_BUNDLE_SHORT_VERSION_STRING "${LM_VERSION}")
set_target_properties(legesmotus PROPERTIES MACOSX_BUNDLE_LONG_VERSION_STRING "${LM_VERSION}")
set_target_properties(legesmotus PROPERTIES MACOSX_BUNDLE_BUNDLE_VERSION "${LM_VERSION}")
endif()
endif()

if(CFLAGS)
set_target_properties(lmgui PROPERTIES COMPILE_FLAGS "${CFLAGS}")
endif()

if(LDFLAGS)
set_target_properties(legesmotus PROPERTIES LINK_FLAGS "${LDFLAGS}")
endif()

target_link_libraries(legesmotus lmgui lmclient lmcommon ${LIBS})
