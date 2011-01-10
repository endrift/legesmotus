BASEDIR = ..
NOBUNDLE = 1
include $(BASEDIR)/common.mk
TESTOBJS = test_primitives test_widgets test_label test_blend test_images test_graphics test_gameview \
	test_rendering test_shaders
CXXFLAGS += $(CLIENTFLAGS)
LIBS := $(CLIENTLIBS)

all: $(TESTOBJS)

%: %.cpp ../liblmgui.a ../liblmclient.a ../liblmcommon.a ../liblmserver.a
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

clean:
	@rm -f $(TESTOBJS)
