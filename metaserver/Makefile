BASEDIR = ..
OBJS = MetaServer.o #put .o filenames here (DO NOT PUT .cpp!)
include ../common.mk

all: lmmetaserver

lmmetaserver: $(OBJS) main.o ../liblmcommon.a
	$(CXX) $(LDFLAGS) -o lmmetaserver $^ $(LIBS)

clean: common-clean
	$(RM) $(OBJS) main.o *~

deps: common-deps
