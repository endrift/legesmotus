BASEDIR = ..
include ../Makefile.in
OBJS = ClientNetwork.o GameController.o GameWindow.o #put .o filenames here (DO NOT PUT .cpp!)

FLAGS_SDL = $(shell sdl-config --cflags)
LIBS_SDL = $(shell sdl-config --libs) -lSDL_image -lSDL_ttf -lSDL_net -lSDL_mixer

ifeq ($(MACHINE),Darwin)
FLAGS_GL = -FOpenGL
LIBS_GL = -framework OpenGL
else
LIBS_GL = -lGL
endif

INCLUDES = -I..

CXXFLAGS += $(FLAGS_SDL) $(FLAGS_GL) $(INCLUDES)
LDFLAGS += $(LIBS_SDL) $(LIBS_GL)

all: legesmotus

legesmotus: ../liblmclient.a ../liblmcommon.a main.o
	$(CXX) $(LDFLAGS) -o legesmotus -L.. main.o -llmclient -llmcommon

../liblmclient.a: $(OBJS)
	$(AR) crus ../liblmclient.a $(OBJS)

clean:
	rm -f $(OBJS) *~
