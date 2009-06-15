BASEDIR = ..
include ../Makefile.in
OBJS = ClientNetwork.o ClientSDL.o GameController.o GameWindow.o MapObject.o Sprite.o GraphicalPlayer.o \
	Graphic.o TiledGraphic.o GraphicGroup.o GraphicalMap.o Font.o TextManager.o Text.o Mask.o \
	Minimap.o TableBackground.o ScrollArea.o ScrollBar.o Transition.o Curve.o #put .o filenames here (DO NOT PUT .cpp!)
CXXFLAGS += $(CLIENTFLAGS)

ifeq ($(NOSOUND),)
OBJS += SoundController.o
else
OBJS += SoundController_dummy.o
endif

ifeq ($(MACHINE),Windows)
PLATFORM_DEP = legesmotus-res.o
legesmotus-res.o: legesmotus.ico
endif
ifeq ($(MACHINE)$(UNIXSTYLE),Darwin)
PLATFORM_DEP = mac.o
endif

all: legesmotus

legesmotus: ../liblmclient.a ../liblmcommon.a main.o $(PLATFORM_DEP)
	$(CXX) $(LDFLAGS) -o legesmotus -L.. main.o -llmclient -llmcommon $(CLIENTLIBS) $(PLATFORM_DEP)

../liblmclient.a: $(OBJS)
	$(AR) crus ../liblmclient.a $(OBJS)

clean:
	rm -f $(OBJS) *~ main.o ../liblmclient.a legesmotus $(PLATFORM_DEP)

# ==================================== #

ClientNetwork.o: $(H_CLIENTNETWORK)
ClientSDL.o: $(H_CLIENTSDL)
GameController.o: $(H_GAMECONTROLLER) $(H_GAMEWINDOW) $(H_CLIENTNETWORK)
GameWindow.o: $(H_EXCEPTION) $(H_GAMEWINDOW) $(H_GL)
