BASEDIR = ..
include ../Makefile.in
OBJS = Server.o ServerNetwork.o ServerSDL.o #put .o filenames here (DO NOT PUT .cpp!)

LIBS += -lSDL_net

INCLUDES = -I..

CXXFLAGS += $(INCLUDES)

all: lmserver

lmserver: ../liblmserver.a ../liblmcommon.a main.o
	$(CXX) $(LDFLAGS) $(LIBS) -o lmserver -L.. main.o -llmserver -llmcommon

../liblmserver.a: $(OBJS)
	$(AR) crus ../liblmserver.a $(OBJS)

clean:
	rm -f $(OBJS) main.o *~
