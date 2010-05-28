BASEDIR = ..
NOBUNDLE = 1
include $(BASEDIR)/common.mk
OBJS = test_primitives test_widgets
CXXFLAGS += $(CLIENTFLAGS) -I../client
LIBS := $(CLIENTLIBS)

all: $(OBJS)

%: %.cpp ../liblmclient.a ../liblmcommon.a ../liblmserver.a ../liblmgui.a
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

clean:
	@rm -f $(OBJS)
