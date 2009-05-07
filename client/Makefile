BASEDIR = ..
include ../Makefile.in
OBJS = ClientNetwork.o ClientSDL.o GameController.o GameWindow.o MapObject.o Sprite.o GraphicalPlayer.o \
	Graphic.o TiledGraphic.o GraphicGroup.o GraphicalMap.o Font.o TextManager.o SoundController.o Text.o \
	Mask.o Minimap.o TableBackground.o #put .o filenames here (DO NOT PUT .cpp!)

all: legesmotus

legesmotus: ../liblmclient.a ../liblmcommon.a main.o
	$(CXX) $(LDFLAGS) -o legesmotus -L.. main.o -llmclient -llmcommon $(CLIENTLIBS)

../liblmclient.a: $(OBJS)
	$(AR) crus ../liblmclient.a $(OBJS)

clean:
	rm -f $(OBJS) *~ main.o legesmotus

# ==================================== #

ClientNetwork.o: $(H_CLIENTNETWORK)
ClientSDL.o: $(H_CLIENTSDL)
GameController.o: $(H_GAMECONTROLLER) $(H_GAMEWINDOW) $(H_CLIENTNETWORK)
GameWindow.o: $(H_EXCEPTION) $(H_GAMEWINDOW) $(H_GL)
