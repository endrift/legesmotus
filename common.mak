all:
COMMON = $(BASEDIR)/common
CLIENT = $(BASEDIR)/client
SERVER = $(BASEDIR)/server
FRAMEWORKS = /Library/Frameworks

ifeq ($(SDL_CONFIG),)
 SDL_CONFIG = sdl-config
endif

# Version strings
VERSION = 0.3.0-svn

# These may be overridden by config.mak
DATADIR = data
#UNIVERSAL = 1
#UNIXSTYLE = 1

ifneq ($(shell test -r $(BASEDIR)/config.mak && echo 1),)
 include $(BASEDIR)/config.mak
endif

ifeq ($(SHAREDIR),)
 SHAREDIR = $(PREFIX)/share
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
 FLAGS_SDL = $(shell $(SDL_CONFIG) --cflags)
 LIBS_SDL = $(shell $(SDL_CONFIG) --libs) -lSDL_image -lSDL_ttf
 ifeq ($(NOSOUND),)
  LIBS_SDL += -lSDL_mixer
 endif
endif

ifeq ($(MACHINE),Darwin)
 ifneq ($(UNIVERSAL),)
  CFLAGS += -arch ppc -arch i386
 else
  ifeq ($(UNIXSTYLE),)
   CFLAGS += -DLM_FWBASED
   # Test for Snow Leopard
   ifeq ($(shell test `uname -r | cut -f 1 -d .` -ge 10 && echo 1),1) 
    CFLAGS += -arch i386
   endif
  endif
 endif
 FLAGS_GL = -FOpenGL
 LIBS_GL = -framework OpenGL
else
 ifeq ($(MACHINE),Windows)
  LIBS_GL = -lopengl32
 else
  ifneq ($(shell ls /usr/lib/libGL.so 2>/dev/null),)
   LIBS_GL = -lGL
  else
   ifeq ($(XDIR),)
    ifneq ($(shell ls -d /usr/X11),)
     XDIR = /usr/X11
    else
     ifneq ($(shell ls -d /usr/X11R? 2>/dev/null),)
      XDIR = $(shell ls -1d /usr/X11R? 2>/dev/null | head -n1)
     else
      $(error Cannot detect X11 directory, please specify manually)
     endif
    endif
   endif
   LIBS_GL = -L$(XDIR)/lib/ -lGL
  endif
 endif
endif

INCLUDES = -I$(BASEDIR)

CFLAGS += -Wall -Wnon-virtual-dtor

ifeq ($(DEBUG),1)
 CFLAGS += -g -O0
else
 CFLAGS += -O2
endif

LDFLAGS += $(CFLAGS)
CFLAGS += $(FLAGS_GL) $(INCLUDES)
ifeq ($(MACHINE),Windows)
 LIBS += -lwsock32
endif
CLIENTFLAGS = $(FLAGS_SDL)
CLIENTLIBS = $(LIBS_GL) $(LIBS) $(LIBS_SDL)

CXXFLAGS += $(CFLAGS) -fno-rtti

%.o: %.rc
	windres $< -o $@

.deps/%.d: %.cpp .deps
	$(CXX) -M $(CXXFLAGS) $< | sed -e 's,^\([^:]*\)\.o:,\1.o $@:,' > $@

common-deps: $(PHONY) .deps .deps/deps.mak

common-clean: $(PHONY)
	$(RM) -r .deps

.deps:
	mkdir -p .deps

.deps/deps.mak: $(OBJS:%.o=.deps/%.d)
	cat .deps/*.d > .deps/deps.mak

ifneq ($(OBJS),)
 ifneq ($(shell test -r .deps/deps.mak && echo 1),)
  ifeq ($(NODEPS),)
   include .deps/deps.mak
  endif
 endif
endif
