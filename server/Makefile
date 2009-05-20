BASEDIR = ..
include ../Makefile.in
OBJS = Server.o ServerMap.o ServerNetwork.o ServerPlayer.o ServerSDL.o #put .o filenames here (DO NOT PUT .cpp!)

INCLUDES = -I..

CXXFLAGS += $(INCLUDES)

all: lmserver

lmserver: ../liblmserver.a ../liblmcommon.a main.o
	$(CXX) $(LDFLAGS) -o lmserver -L.. main.o -llmserver -llmcommon $(LIBS)

../liblmserver.a: $(OBJS)
	$(AR) crus ../liblmserver.a $(OBJS)

clean:
	rm -f $(OBJS) main.o ../liblmserver.a *~
