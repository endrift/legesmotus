BASEDIR = ..
OBJS = GateStatus.o Server.o ServerConfig.o ServerMap.o ServerNetwork.o ServerPlayer.o Spawnpoint.o GameModeHelper.o ClassicMode.o DeathmatchMode.o ZombieMode.o #put .o filenames here (DO NOT PUT .cpp!)
include $(BASEDIR)/common.mak

all: lmserver

lmserver: $(BASEDIR)/liblmserver.a $(BASEDIR)/liblmcommon.a main.o
	$(CXX) $(LDFLAGS) -o lmserver -L.. main.o -llmserver -llmcommon $(LIBS)

$(BASEDIR)/liblmserver.a: $(OBJS)
	$(AR) crus $(BASEDIR)/liblmserver.a $(OBJS)

clean: common-clean
	$(RM) $(OBJS) main.o $(BASEDIR)/liblmserver.a lmserver *~

deps: common-deps
