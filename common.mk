all:
.SUFFIXES:
COMMON = $(BASEDIR)/common
CLIENT = $(BASEDIR)/client
SERVER = $(BASEDIR)/server
FRAMEWORKS = /Library/Frameworks

# Version strings
VERSION = 0.4.0-svn

# These may be overridden by config.mk
DATADIR = data
#UNIVERSAL = 1
#NOBUNDLE = 1

-include $(BASEDIR)/config.mk

# Default targets if no targets are explicitly listed
ifeq ($(TARGETS),)
TARGETS = client server
endif

ifeq ($(SDL_CONFIG),)
 SDL_CONFIG = sdl-config
endif

ifeq ($(SHAREDIR),)
 SHAREDIR = $(PREFIX)/share
endif

ifneq ($(NO_UPGRADE_NAG),)
 CFLAGS += -DLM_NO_UPGRADE_NAG
endif

CFLAGS += -DLM_DATA_DIR=\"$(DATADIR)\" -DLM_VERSION="\"$(VERSION)\""

MACHINE ?= $(shell uname -s)
ifeq ($(MACHINE),)
 MACHINE = Windows
else
 ifeq ($(shell echo $(MACHINE) | cut -c 1-5),MINGW)
 MACHINE = Windows
 endif
endif

# Determine platforms we should build for
ifeq ($(ARCHS),)
 ifeq ($(MACHINE),Windows)
  ARCHS += i386
 else
  ifeq ($(MACHINE),Darwin)
   ifeq ($(NOBUNDLE),)
    ifeq ($(UNIVERSAL),)
     ARCHS += $(shell arch)
    else
     ARCHS += ppc
     ARCHS += i386
    endif
   else
    IS_X64 = $(shell lipo `sdl-config --prefix`/lib/libSDL.a -verify_arch x86_64 && echo x86_64)
    IS_X86 = $(shell lipo `sdl-config --prefix`/lib/libSDL.a -verify_arch i386 && echo i386)
    IS_PPC = $(shell lipo `sdl-config --prefix`/lib/libSDL.a -verify_arch ppc && echo ppc)
    ifeq ($(UNIVERSAL),)
     ARCHS += $(IS_PPC) $(IS_X64) $(IS_X86)
    else
     ifneq ($(IS_X64),)
      OUR_ARCH ?= x86_64
     endif
     ifneq ($(IS_X86),)
      OUR_ARCH ?= i386
     endif
     ifneq ($(IS_PPC),)
      OUR_ARCH ?= ppc
     endif
     ARCHS += $(OUR_ARCH)
    endif
   endif
  else
   ARCHS += $(shell arch)
  endif
 endif
endif

ifneq ($(filter $(ARCH),$(ARCHS)),$(ARCH))
 ifneq ($(MACHINE)-$(ARCH),Darwin-universal)
  $(error Disallowed architecture for this platform: $(ARCH))
 endif
endif

# BSD sed does not like -i'' for in-place, but Linux does not like -i ''
ifeq ($(MACHINE),Linux)
 INPLACE = -i''
else
 INPLACE = -i ''
endif

ifeq ($(MACHINE)$(NOBUNDLE),Darwin)
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
 # is Mac
 ifeq ($(NOBUNDLE),)
  CFLAGS += -DLM_FWBASED
 endif
 ifneq ($(UNIVERSAL),)
  ifeq ($(ARCH),)
   CFLAGS += -arch ppc -arch i386
  else
   CFLAGS += -arch $(ARCH)
  endif
 else
  ifeq ($(NOBUNDLE),)
   # Test for Snow Leopard (SDL TTF's stock release is 32-bit)
   ifeq ($(shell test `uname -r | cut -f 1 -d .` -ge 10 && echo 1),1) 
    CFLAGS += -arch i386
   endif
  endif
 endif
 FLAGS_GL = -FOpenGL
 LIBS_GL = -framework OpenGL
else
 ifneq ($(findstring client,$(TARGETS)),)
  # We need graphics libraries
  ifeq ($(MACHINE),Windows)
   LIBS_GL = -lopengl32
  else
   # Generic Unix
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
endif

INCLUDES = -I$(BASEDIR)

CFLAGS += -Wall
CXXFLAGS += -Wnon-virtual-dtor

ifeq ($(DEBUG),1)
 RELEASE=debug
 CFLAGS += -g -O0
else
 RELEASE=release
 CFLAGS += -O2
endif

LDFLAGS += $(CFLAGS)
CFLAGS += $(FLAGS_GL) $(INCLUDES)
ifeq ($(MACHINE),Windows)
 LIBS += -lwsock32
endif
CLIENTFLAGS = $(FLAGS_SDL) $(shell freetype-config --cflags)
CLIENTLIBS = $(LIBS_GL) $(LIBS) $(LIBS_SDL) $(shell freetype-config --libs)

CXXFLAGS += $(CFLAGS)

ifneq ($(SUBDIR),)
 #SRCDIR = $(subst \ ,?,$(BASEDIR)/$(SUBDIR))
 SRCDIR = $(BASEDIR)/$(SUBDIR)
 vpath %.c $(SRCDIR)
 vpath %.cpp $(SRCDIR)
 vpath %.m $(SRCDIR)
 vpath %.o $(SRCDIR)
 #VPATH = $(SRCDIR)
endif

%.a:
	$(AR) crus $@ $^

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

%.o: %.m
	$(CC) -c $(CFLAGS) $< -o $@

%.o: %.rc
	windres $< -o $@

.deps/%.d: %.cpp .deps
	$(CXX) -M $(CXXFLAGS) $< | sed -e 's,^\([^:]*\)\.o:,\1.o $@:,' > $@

common-deps: .deps .deps/deps.mk

common-tidy:
	@$(RM) -r *.{o,dSYM}
	@$(RM) *~

common-clean: common-tidy
	@$(RM) -r *.{a,dmg,app}
	@$(RM) -r .deps

tidy: common-tidy
clean: common-clean

.deps:
	mkdir -p .deps

.deps/deps.mk: $(OBJS:%.o=.deps/%.d)
	cat .deps/*.d > .deps/deps.mk

ifneq ($(filter clean,$(MAKECMDGOALS)),clean)
 ifneq ($(OBJS),)
  #-include .deps/deps.mk
 endif
endif
