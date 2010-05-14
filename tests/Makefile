BASEDIR = ..
UNIXSTYLE = 1
include $(BASEDIR)/common.mak
OBJS = test_primitives
CXXFLAGS += $(CLIENTFLAGS)
LIBS := $(CLIENTLIBS) -L$(BASEDIR) -llmclient -llmserver -llmcommon -llmgui

all: $(BASEDIR)/liblmclient.a $(BASEDIR)/liblmcommon.a $(BASEDIR)/liblmserver.a $(BASEDIR)/liblmgui.a $(OBJS)

%: %.cpp 
	$(CXX) $(CXXFLAGS) -o $@ $< $(LIBS)

clean:
	rm -f $(OBJS) *~
	rm -rf *.dSYM
