BASEDIR = ..
OBJS = MetaServer.o #put .o filenames here (DO NOT PUT .cpp!)
include ../common.mak

all: lmmetaserver

lmmetaserver: $(OBJS) ../liblmcommon.a main.o
	$(CXX) $(LDFLAGS) -o lmmetaserver -L.. main.o $(OBJS) -llmcommon $(LIBS)

clean: common-clean
	$(RM) $(OBJS) main.o *~

deps: common-deps
