# CMake list for Leges Motus client

cmake_minimum_required(VERSION 2.6)

find_package(OpenGL REQUIRED)
find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)
find_package(SDL_ttf REQUIRED)

include_directories(${SDL_INCLUDE_DIR} ${SDLIMAGE_INCLUDE_DIR} ${SDLTTF_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})

if(CMAKE_HOST_APPLE)
	list(APPEND PLATFORM_SRC mac.m)
	if(FLAG_NOBUNDLE)
		execute_process(COMMAND ${FLAG_OPT_USR}/bin/sdl-config --cflags OUTPUT_VARIABLE SDL_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
		execute_process(COMMAND ${FLAG_OPT_USR}/bin/sdl-config --libs OUTPUT_VARIABLE SDL_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)
		set(CXXFLAGS "${CXXFLAGS} ${SDL_FLAGS}")
		set(LDFLAGS "${LDFLAGS} ${SDL_LIBS}")
	endif(FLAG_NOBUNDLE)
endif(CMAKE_HOST_APPLE)

if(FLAG_NOSOUND)
	add_definitions(-DLM_NOSOUND)
	list(APPEND OPT_SRC SoundController_dummy.cpp)
else(FLAG_NOSOUND)
	find_package(SDL_mixer REQUIRED)
	include_directories(${SDLMIXER_INCLUDE_DIR})
	list(APPEND OPT_SRC SoundController.cpp)
	list(APPEND OPT_LIB ${SDLMIXER_LIBRARY})
endif(FLAG_NOSOUND)

add_library(lmclient STATIC
	ClientNetwork.cpp
	ClientSDL.cpp
	GameController.cpp
	GameWindow.cpp
	Sprite.cpp
	GraphicalPlayer.cpp
	Graphic.cpp
	TiledGraphic.cpp
	GraphicGroup.cpp
	GraphicalMap.cpp
	Font.cpp
	TextManager.cpp
	Text.cpp
	Mask.cpp
	Radar.cpp
	TableBackground.cpp
	ScrollArea.cpp
	ScrollBar.cpp
	Transition.cpp
	Curve.cpp
	TransitionManager.cpp
	ClientConfiguration.cpp
	MapReceiver.cpp
	ServerBrowser.cpp
	ChatLog.cpp
	RadialBackground.cpp
	ConvolveKernel.cpp
	MapObjectParams.cpp
	BaseMapObject.cpp
	Obstacle.cpp
	Decoration.cpp
	ClientGate.cpp
	ClientMap.cpp
	RepulsionField.cpp
	Menu.cpp
	MenuItem.cpp
	TextMenuItem.cpp
	ArbitraryMenu.cpp
	ForceField.cpp
	Weapon.cpp
	StandardGun.cpp
	SpreadGun.cpp
	ImpactCannon.cpp
	Hazard.cpp
	Form.cpp
	FormItem.cpp
	ListMenuItem.cpp
	RadialMenu.cpp
	GraphicMenuItem.cpp
	TextInput.cpp
	GraphicsCache.cpp
	ThawGun.cpp
	${OPT_SRC})

add_executable(legesmotus WIN32 ${PLATFORM_SRC} main.cpp)
add_dependencies(legesmotus lmcommon lmclient)
set_target_properties(lmclient legesmotus PROPERTIES COMPILE_FLAGS ${CXXFLAGS})
set_target_properties(lmclient legesmotus PROPERTIES LINK_FLAGS ${LDFLAGS})
target_link_libraries(legesmotus lmcommon lmclient ${OPENGL_gl_LIBRARY} ${SDL_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDLTTF_LIBRARY} ${OPT_LIB})

if(DEFINED ENV{DESTDIR})
	get_filename_component(DESTDIR "$ENV{DESTDIR}" ABSOLUTE)
	message(STATUS "Staging to ${DESTDIR}")
endif(DEFINED ENV{DESTDIR})

if(CMAKE_HOST_APPLE AND NOT FLAG_NOBUNDLE)
	set(MKAPP "mkdir -p \"Leges Motus.app/Contents/MacOS\""
		"mkdir -p \"Leges Motus.app/Contents/Resources\""
		"mkdir -p \"Leges Motus.app/Contents/Frameworks\""
		"cp -Rf \"${DATA_SRC}\" \"Leges Motus.app/Contents/Resources\""
		"cp -Rf \"${CLIENT_DIR}\"/legesmotus.{icns,nib} \"Leges Motus.app/Contents/Resources\""
		"cp -f legesmotus \"Leges Motus.app/Contents/MacOS\""
		"cp -f \"${CLIENT_DIR}\"/Info.plist \"Leges Motus.app/Contents/\""
		"sed -e \"s/\\$VERSION/${VERSION}/\" -i \"\" \"Leges Motus.app/Contents/Info.plist\""
		"find \"Leges Motus.app\" -name .svn -print0 | xargs -0 rm -rf"
		"test -d \"Leges Motus.app/Contents/Frameworks/SDL.framework\" || cp -Rf /Library/Frameworks/SDL.framework \"Leges Motus.app/Contents/Frameworks\""
		"test -d \"Leges Motus.app/Contents/Frameworks/SDL_image.framework\" || cp -Rf /Library/Frameworks/SDL_image.framework \"Leges Motus.app/Contents/Frameworks\""
		"test -d \"Leges Motus.app/Contents/Frameworks/SDL_ttf.framework\" || cp -Rf /Library/Frameworks/SDL_ttf.framework \"Leges Motus.app/Contents/Frameworks\""
		"test -d \"Leges Motus.app/Contents/Frameworks/SDL_mixer.framework\" || cp -Rf /Library/Frameworks/SDL_mixer.framework \"Leges Motus.app/Contents/Frameworks\"")

	add_custom_command(TARGET legesmotus POST_BUILD COMMAND bash -c "${MKAPP}" COMMENT "Creating App Bundle" VERBATIM)

	add_executable("Leges Motus.app" IMPORTED)
	install(TARGETS "Leges Motus.app" DESTINATION "${DESTDIR}/Applications")
elseif(CMAKE_HOST_UNIX)

endif()
