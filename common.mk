all:
.SUFFIXES:
COMMON = $(BASEDIR)/common
CLIENT = $(BASEDIR)/client
SERVER = $(BASEDIR)/server
FRAMEWORKS = /Library/Frameworks

# Version strings
VERSION = 0.4.0

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
# Windows is the only supported platform that doesn't have uname
ifeq ($(MACHINE),)
 MACHINE = Windows
else
 # Unless it's MSYS
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
     ARCHS += $(shell uname -m)
    else
     ARCHS += ppc
     ARCHS += i386
    endif
   else
    # Does our SDL installation support these platforms?
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
   ARCHS += $(shell uname -m)
  endif
 endif
endif

# Only Mac OS X can use fat binaries
ifneq ($(filter $(ARCH),$(ARCHS)),$(ARCH))
 ifneq ($(MACHINE)-$(ARCH),Darwin-universal)
  $(error Disallowed architecture for this platform: $(ARCH))
 endif
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
 # Solaris needs extra libraries for BSD sockets
 ifeq ($(MACHINE),SunOS)
  LIBS += -lnsl -lsocket -lresolv
 endif

 # Grab SDL if we need it
 ifneq ($(findstring client,$(TARGETS)),)
  FLAGS_SDL := $(shell $(SDL_CONFIG) --cflags)
  LIBS_SDL := $(shell $(SDL_CONFIG) --libs) -lSDL_image -lSDL_ttf
  ifeq ($(NOSOUND),)
   LIBS_SDL += -lSDL_mixer
  endif
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
   # Test for Snow Leopard (SDL_TTF's stock framework is 32-bit)
   ifeq ($(shell test `uname -r | cut -f 1 -d .` -ge 10 && echo 1),1) 
    CFLAGS += -arch i386
   endif
  endif
 endif
 FLAGS_GL = -FOpenGL
 LIBS_GL = -framework OpenGL
else
 ifneq ($(findstring client,$(TARGETS)),)
  # We're bulding the client
  # We need graphics libraries
  ifeq ($(MACHINE),Windows)
   LIBS_GL = -lopengl32
  else
   # Generic Unix
   ifneq ($(shell ls /usr/lib/libGL.so 2>/dev/null),)
    LIBS_GL = -lGL
   else
    ifeq ($(XDIR),)
     ifneq ($(shell ls -d /usr/X11 2>/dev/null),)
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
 RELEASE = debug
 CFLAGS += -g -O0 -DLM_DEBUG
else
 RELEASE = release
 # Take O-flag from CFLAGS if we already have it set
 ifneq ($(filter -O%,$(CFLAGS)),)
  CFLAGS += -O2
 endif
endif

LDFLAGS += $(CFLAGS)
CFLAGS += $(FLAGS_GL) $(INCLUDES)

# Windows needs Winsock2 for BSD sockets
ifeq ($(MACHINE),Windows)
 LIBS += -lwsock32
endif

CLIENTFLAGS = $(FLAGS_SDL)
CLIENTLIBS = $(LIBS_GL) $(LIBS) $(LIBS_SDL)

CXXFLAGS += $(CFLAGS)

# Out-of-tree build madness
ifneq ($(SUBDIR),)
 #SRCDIR = $(subst \ ,?,$(BASEDIR)/$(SUBDIR))
 SRCDIR = $(BASEDIR)/$(SUBDIR)
 VPATH = $(SRCDIR)
endif

BINOBJS = $(foreach obj,$(BINSRCS),$(obj).o)
LIBOBJS = $(foreach obj,$(LIBSRCS),$(obj).o)
OBJS = $(BINOBJS) $(LIBOBJS)

ifneq ($(LIBRARY),)
$(LIBRARY): $(LIBOBJS)
	$(AR) crus $(LIBRARY) $^
endif

%.a:
	$(AR) crus $@ $^

%.cpp.o: %.cpp %.cpp.d
	$(CXX) -c $(CXXFLAGS) $< -o $@

%.m.o: %.m %.m.d
	$(CC) -c $(CFLAGS) $< -o $@

%.rc.o: %.rc
	windres $< -o $@

%.cpp.d: %.cpp
	$(CXX) -M $(CXXFLAGS) $< | sed -e 's,^\([^:]*\)\.o:,\1.o $@:,' > $@

%.m.d: %.m
	$(CC) -M $(CFLAGS) $< | sed -e 's,^\([^:]*\)\.o:,\1.o $@:,' > $@

common-deps: deps.mk

common-tidy:
	@$(RM) -r *.{o,dSYM}
	@$(RM) *~

common-clean: common-tidy
	@$(RM) -r *.{a,dmg,app}
	@$(RM) *.d

tidy: common-tidy
clean: common-clean

ifneq ($(ARCH),)
# Don't touch deps if we're not in a chdir

deps.mk: $(OBJS:%.o=%.d)
	cat *.d > deps.mk

 # Don't build deps on make clean
 ifneq ($(filter clean,$(MAKECMDGOALS)),clean)
  ifneq ($(OBJS),)
   -include deps.mk
  endif
 endif
endif
