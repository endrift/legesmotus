if(CMAKE_HOST_APPLE)

if(LM_BUNDLE)
else()
set($ENV{SDLDIR} /opt/local)
endif()

endif()

find_package(SDL REQUIRED)
include_directories(${SDL_INCLUDE_DIR})
set(LIBS ${LIBS} ${SDLMAIN_LIBRARY} ${SDL_LIBRARY})

find_package(SDL_image REQUIRED)
include_directories(${SDLIMAGE_INCLUDE_DIR})
set(LIBS ${LIBS} ${SDLIMAGE_LIBRARY})

find_package(Freetype REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIR_ft2build} ${FREETYPE_INCLUDE_DIR_freetype2})
set(LIBS ${LIBS} ${FREETYPE_LIBRARIES})

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

if(${CMAKE_HOST_APPLE})
set_target_properties(legesmotus PROPERTIES LINK_FLAGS -Wl,-framework,Cocoa)
endif()

target_link_libraries(legesmotus lmgui lmclient lmcommon ${LIBS} Box2D)
