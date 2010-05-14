BASEDIR = ..
UNIXSTYLE = 1
include ../common.mak
OBJS = test_primitives
CXXFLAGS += $(CLIENTLIBS) $(CLIENTFLAGS) -L.. -llmclient -llmserver -llmcommon -llmgui

all: ../liblmclient.a ../liblmcommon.a ../liblmserver.a ../liblmgui.a $(OBJS)

%: %.cpp 
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(OBJS) *~
	rm -rf *.dSYM
