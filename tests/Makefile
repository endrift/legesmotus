BASEDIR = ..
NOBUNDLE = 1
include $(BASEDIR)/common.mk
OBJS = test_primitives test_widgets test_label test_drawmodes
CXXFLAGS += $(CLIENTFLAGS) $(shell freetype-config --cflags) -I../client
LIBS := $(CLIENTLIBS) $(shell freetype-config --libs)

all: $(OBJS)

%: %.cpp ../liblmgui.a ../liblmclient.a ../liblmcommon.a ../liblmserver.a
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

clean:
	@rm -f $(OBJS)
