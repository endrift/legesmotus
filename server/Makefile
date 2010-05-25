BASEDIR = ..
OBJS = GateStatus.o Server.o ServerConfig.o ServerMap.o ServerNetwork.o ServerPlayer.o Spawnpoint.o GameModeHelper.o ClassicMode.o DeathmatchMode.o ZombieMode.o #put .o filenames here (DO NOT PUT .cpp!)
include $(BASEDIR)/common.mk

all: lmserver

lmserver: ../liblmserver.a ../liblmcommon.a main.o
	$(CXX) $(LDFLAGS) -o lmserver $^ $(LIBS)

../liblmserver.a: $(OBJS)

clean: common-clean
	$(RM) $(OBJS) main.o ../liblmserver.a lmserver *~

deps: common-deps
