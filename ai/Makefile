BASEDIR = ..
LIBSRCS := ReactiveAIController.cpp StateTranslator.cpp
BINSRCS := main.cpp
LIBRARY := ../liblmai.a

include $(BASEDIR)/common.mk

all: $(LIBRARY) reactiveai

reactiveai: $(BINOBJS) $(LIBRARY) ../liblmclient.a ../liblmcommon.a
	$(CXX) $(LDFLAGS) -o lmreactiveai $^ $(LIBS)

clean: common-clean
	@$(RM) $(LIBRARY)

deps: common-deps
