BASEDIR = ..
LIBSRCS := ReactiveAIController.cpp
BINSRCS := main.cpp
LIBRARY := ../liblmai.a

include $(BASEDIR)/common.mk

# Stopgap measure until client separated out
CXXFLAGS += $(CLIENTFLAGS) $(shell freetype-config --cflags)
CXXFLAGS += -I../client

all: $(LIBRARY) reactiveai

reactiveai: $(BINOBJS) $(LIBRARY) ../liblmclient.a ../liblmcommon.a
	$(CXX) $(LDFLAGS) -o lmreactiveai $^ $(CLIENTLIBS)

clean: common-clean
	@$(RM) $(LIBRARY)

deps: common-deps
