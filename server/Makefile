BASEDIR = ..
LIBSRCS := GateStatus.cpp Server.cpp ServerConfig.cpp ServerMap.cpp ServerNetwork.cpp ServerPlayer.cpp Spawnpoint.cpp \
	GameModeHelper.cpp ClassicMode.cpp DeathmatchMode.cpp ZombieMode.cpp
BINSRCS := main.cpp
LIBRARY := ../liblmserver.a

include $(BASEDIR)/common.mk

all: lmserver

lmserver: $(BINOBJS) $(LIBRARY) ../liblmcommon.a
	$(CXX) $(LDFLAGS) -o lmserver $^ $(LIBS)

clean: common-clean
	@$(RM) $(LIBRARY) lmserver

deps: common-deps
