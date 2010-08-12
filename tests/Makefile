BASEDIR = ..
NOBUNDLE = 1
include $(BASEDIR)/common.mk
TESTOBJS = 
CXXFLAGS += $(CLIENTFLAGS) $(shell freetype-config --cflags) -I../client
LIBS := $(CLIENTLIBS) $(shell freetype-config --libs)

all: $(TESTOBJS)

%: %.cpp ../liblmclient.a ../liblmcommon.a ../liblmserver.a
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

clean:
	@rm -f $(TESTOBJS)
