BASEDIR = ..
NOBUNDLE = 1
include $(BASEDIR)/common.mk
TESTOBJS = 
CXXFLAGS += $(CLIENTFLAGS) -I../client
LIBS := $(CLIENTLIBS) 

all: $(TESTOBJS)

%: %.cpp ../liblmclient.a ../liblmcommon.a ../liblmserver.a
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

clean:
	@rm -f $(TESTOBJS)
