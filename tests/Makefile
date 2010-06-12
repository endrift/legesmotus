BASEDIR = ..
NOBUNDLE = 1
include $(BASEDIR)/common.mk
OBJS = test_primitives test_widgets test_label
CXXFLAGS += $(CLIENTFLAGS) -I../client
LIBS := $(CLIENTLIBS)

all: $(OBJS)

%: %.cpp ../liblmgui.a ../liblmclient.a ../liblmcommon.a ../liblmserver.a
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

clean:
	@rm -f $(OBJS)
