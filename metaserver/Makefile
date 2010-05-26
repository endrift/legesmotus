BASEDIR = ..
OBJS = MetaServer.o #put .o filenames here (DO NOT PUT .cpp!)
include $(BASEDIR)/common.mk

all: lmmetaserver

lmmetaserver: $(OBJS) main.o ../liblmcommon.a
	$(CXX) $(LDFLAGS) -o lmmetaserver $^ $(LIBS)

deps: common-deps
