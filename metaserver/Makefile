BASEDIR = ..
include ../common.mak
OBJS = MetaServer.o #put .o filenames here (DO NOT PUT .cpp!)
-include $(OBJS:%.o=.deps/%.d)

all: lmmetaserver

lmmetaserver: $(OBJS) ../liblmcommon.a main.o
	$(CXX) $(LDFLAGS) -o lmmetaserver -L.. main.o $(OBJS) -llmcommon $(LIBS)

clean:
	rm -f $(OBJS) main.o *~

deps: common-deps
