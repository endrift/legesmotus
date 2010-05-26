BASEDIR = ..
NOBUNDLE = 1
include $(BASEDIR)/common.mk
OBJS = test_primitives
CXXFLAGS += $(CLIENTFLAGS) -I../client
LIBS := $(CLIENTLIBS) -L$(BASEDIR) -llmclient -llmserver -llmcommon -llmgui

all: ../liblmclient.a ../liblmcommon.a ../liblmserver.a ../liblmgui.a $(OBJS)

%: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LIBS)

clean:
	@rm -f $(OBJS)
