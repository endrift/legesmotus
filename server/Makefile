BASEDIR = ..
OBJS = GateStatus.o Server.o ServerConfig.o ServerMap.o ServerNetwork.o ServerPlayer.o Spawnpoint.o #put .o filenames here (DO NOT PUT .cpp!)
include ../common.mak

all: lmserver

lmserver: ../liblmserver.a ../liblmcommon.a main.o
	$(CXX) $(LDFLAGS) -o lmserver -L.. main.o -llmserver -llmcommon $(LIBS)

../liblmserver.a: $(OBJS)
	$(AR) crus ../liblmserver.a $(OBJS)

clean: common-clean
	rm -f $(OBJS) main.o ../liblmserver.a *~

deps: common-deps
