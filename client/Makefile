BASEDIR = ..
include ../Makefile.in
OBJS = ClientNetwork.o ClientSDL.o GameController.o GameWindow.o #put .o filenames here (DO NOT PUT .cpp!)

LIBS += -lSDL_image -lSDL_ttf -lSDL_net -lSDL_mixer

ifeq ($(MACHINE),Darwin)
FLAGS_GL = -FOpenGL
LIBS_GL = -framework OpenGL
else
LIBS_GL = -lGL
ifneq ($(MACHINE),Windows)
LIBS_GL = -L/usr/X11/lib/
endif
endif

INCLUDES = -I..

CXXFLAGS += $(FLAGS_SDL) $(FLAGS_GL) $(INCLUDES)
LDFLAGS += $(LIBS_SDL) $(LIBS_GL)

all: legesmotus

legesmotus: ../liblmclient.a ../liblmcommon.a main.o
	$(CXX) $(LDFLAGS) $(LIBS) -o legesmotus -L.. main.o -llmclient -llmcommon

../liblmclient.a: $(OBJS)
	$(AR) crus ../liblmclient.a $(OBJS)

clean:
	rm -f $(OBJS) *~ main.o legesmotus
