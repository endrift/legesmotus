BASEDIR = ..
include ../Makefile.in
OBJS = #put .o filenames here (DO NOT PUT .cpp!)

FLAGS_SDL = $(shell sdl-config --cflags)
LIBS_SDL = $(shell sdl-config --libs) -lSDL_image -lSDL_ttf -lSDL_net -lSDL_mixer

CXXFLAGS += $(FLAGS_SDL)
LDFLAGS += $(LIBS_SDL)

all: legesmotus

legesmotus: ../liblmclient.a main.o
	$(CXX) $(LDFLAGS) -o legesmotus -L.. -llmclient -llmcommon main.o

../liblmclient.a: $(OBJ)
	$(AR) crus liblmclient.a $(OBJ)

clean:
	rm -f $(OBJ) *~
