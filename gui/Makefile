BASEDIR = ..
LIBSRCS := Widget.cpp DrawContext.cpp GLESContext.cpp GuiWindow.cpp Font.cpp Label.cpp Image.cpp ResourceCache.cpp \
	GraphicContainer.cpp Graphic.cpp Sprite.cpp Bone.cpp GraphicRegion.cpp Window.cpp SDLWindow.cpp GuiClient.cpp \
	GraphicalMap.cpp GraphicalPlayer.cpp InputDriver.cpp SDLInputDriver.cpp InputSink.cpp HumanController.cpp \
	GameView.cpp input.cpp GraphicalMapObject.cpp ShaderSet.cpp GLESProgram.cpp Bindings.cpp PhysicsDraw.cpp \
	GraphicalGate.cpp GraphicalWeapon.cpp ConvolveKernel.cpp Hud.cpp pubsub.cpp ProgressBar.cpp Particle.cpp \
	ParticleEmitter.cpp ParticleManager.cpp SimpleRadialEmitter.cpp SimpleLineEmitter.cpp
BINSRCS := main.cpp
LIBRARY := ../liblmgui.a

include $(BASEDIR)/common.mk

CXXFLAGS += $(CLIENTFLAGS)
CXXFLAGS += -I../client

all: $(LIBRARY) legesmotus

legesmotus: $(BINOBJS) $(LIBRARY) ../liblmclient.a ../liblmcommon.a
	$(CXX) $(LDFLAGS) -o legesmotus $^ $(CLIENTLIBS)

clean: common-clean
	@$(RM) $(LIBRARY)

deps: common-deps
