LIBSRCS := ReactiveAIController.cpp AI.cpp FuzzyLogic.cpp FuzzyCategory.cpp FuzzyEnvironment.cpp AIController.cpp \
	FuzzyLogicAI.cpp SparseIntersectMap.cpp MapGrapher.cpp Pathfinder.cpp FuzzyLogicFSM.cpp FuzzyLogicState.cpp
BINSRCS := simplemain.cpp fuzzyaimain.cpp
LIBRARY := ../liblmai.a

include $(BASEDIR)/common.mk

all: $(LIBRARY) simpleai fuzzylogicai

simpleai: simplemain.cpp.o $(LIBRARY) ../liblmclient.a ../liblmcommon.a
	$(CXX) $(LDFLAGS) -o lmsimpleai $^ $(LIBS)

fuzzylogicai: fuzzyaimain.cpp.o $(LIBRARY) ../liblmclient.a ../liblmcommon.a
	$(CXX) $(LDFLAGS) -o lmfuzzylogic $^ $(LIBS)

clean: common-clean
	@$(RM) $(LIBRARY)

deps: common-deps
