all:
COMMON = $(BASEDIR)/common
CLIENT = $(BASEDIR)/client
SERVER = $(BASEDIR)/server
FRAMEWORKS = /Library/Frameworks

# Version strings
VERSION = 0.1.1-svn

# These may be overridden by config.mak
DATADIR = data
#UNIVERSAL = 1
#UNIXSTYLE = 1

OPTS = $(shell test -r $(BASEDIR)/config.mak && echo 1)
ifneq ($(OPTS),)
include $(BASEDIR)/config.mak
endif

CFLAGS += -DLM_DATA_DIR=\"$(DATADIR)\" -DLM_VERSION="\"$(VERSION)\""

MACHINE = $(shell uname -s)
ifeq ($(MACHINE),)
MACHINE = Windows
else
ifeq ($(shell echo $(MACHINE) | cut -c 1-5),MINGW)
MACHINE = Windows
endif
endif

ifeq ($(MACHINE)$(UNIXSTYLE),Darwin)
export MACOSX_DEPLOYMENT_TARGET=10.4
FLAGS_SDL  = -I$(FRAMEWORKS)/SDL.framework/Headers
FLAGS_SDL += -I$(FRAMEWORKS)/SDL_image.framework/Headers
FLAGS_SDL += -I$(FRAMEWORKS)/SDL_ttf.framework/Headers
FLAGS_SDL += -I$(FRAMEWORKS)/SDL_mixer.framework/Headers
LIBS += -framework AppKit
LIBS_SDL = -framework SDL -framework SDL_image -framework SDL_ttf
ifeq ($(NOSOUND),)
LIBS_SDL += -framework SDL_mixer
endif
else
ifeq ($(MACHINE),SunOS)
LIBS += -lnsl -lsocket -lresolv
endif
FLAGS_SDL = $(shell sdl-config --cflags)
LIBS_SDL = $(shell sdl-config --libs) -lSDL_image -lSDL_ttf
ifeq ($(NOSOUND),)
LIBS_SDL += -lSDL_mixer
endif
endif

ifeq ($(MACHINE),Darwin)
ifneq ($(UNIVERSAL),)
CFLAGS += -arch ppc -arch i386
endif
FLAGS_GL = -FOpenGL
LIBS_GL = -framework OpenGL
else
ifeq ($(MACHINE),Windows)
LIBS_GL = -lopengl32
else
LIBS_GL = -L/usr/X11/lib/ -lGL
endif
endif

INCLUDES = -I$(BASEDIR)

CFLAGS += -Wall

ifeq ($(DEBUG),1)
CFLAGS += -g -O0
else
CFLAGS += -s -O2
endif

LDFLAGS += $(CFLAGS)
CFLAGS += $(FLAGS_GL) $(INCLUDES)
ifeq ($(MACHINE),Windows)
LIBS += -lwsock32
endif
CLIENTFLAGS = $(FLAGS_SDL)
CLIENTLIBS = $(LIBS_GL) $(LIBS) $(LIBS_SDL)

CXXFLAGS += $(CFLAGS) -fno-rtti

H_GL	= $(CLIENT)/compat_gl.h

H_LMEXCEPTION	= $(COMMON)/Exception.hpp

H_CLIENTNETWORK		= $(CLIENT)/ClientNetwork.hpp
H_CLIENTSDL		= $(CLIENT)/ClientSDL.hpp
H_GAMECONTROLLER	= $(CLIENT)/GameController.hpp
H_GAMEWINDOW		= $(CLIENT)/GameWindow.hpp

H_GAMEWINDOW		+= $(H_GL)
H_GAMECONTROLLER	+= $(H_GAMEWINDOW) $(H_CLIENTNETWORK)

%.o: %.rc
	windres $< -o $@
