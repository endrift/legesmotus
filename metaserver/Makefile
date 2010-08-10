BASEDIR = ..
BINSRCS = MetaServer.cpp main.cpp

include $(BASEDIR)/common.mk

all: lmmetaserver

lmmetaserver: $(BINOBJS) ../liblmcommon.a
	$(CXX) $(LDFLAGS) -o lmmetaserver $^ $(LIBS)

deps: common-deps
