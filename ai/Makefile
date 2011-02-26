LIBSRCS := ReactiveAIController.cpp StateTranslator.cpp AI.cpp FuzzyLogic.cpp FuzzyCategory.cpp FuzzyEnvironment.cpp AIController.cpp FuzzyLogicAI.cpp
BINSRCS := main.cpp
LIBRARY := ../liblmai.a

include $(BASEDIR)/common.mk

all: $(LIBRARY) reactiveai

reactiveai: $(BINOBJS) $(LIBRARY) ../liblmclient.a ../liblmcommon.a
	$(CXX) $(LDFLAGS) -o lmsimpleai $^ $(LIBS)

clean: common-clean
	@$(RM) $(LIBRARY)

deps: common-deps
