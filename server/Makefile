BASEDIR = ..
include ../common.mak
OBJS = GateStatus.o Server.o ServerConfig.o ServerMap.o ServerNetwork.o ServerPlayer.o Spawnpoint.o #put .o filenames here (DO NOT PUT .cpp!)

all: lmserver

lmserver: ../liblmserver.a ../liblmcommon.a main.o
	$(CXX) $(LDFLAGS) -o lmserver -L.. main.o -llmserver -llmcommon $(LIBS)

../liblmserver.a: $(OBJS)
	$(AR) crus ../liblmserver.a $(OBJS)

clean:
	rm -f $(OBJS) main.o ../liblmserver.a *~
