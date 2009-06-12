BASEDIR = ..
UNIXSTYLE = 1
include ../Makefile.in
OBJS = client_network config_test simple_sprite simple_text simple_scroll

all: $(OBJS)

%: %.cpp ../liblmclient.a ../liblmcommon.a ../liblmserver.a
	$(CXX) $(CXXFLAGS) $(CLIENTFLAGS) $(CLIENTLIBS) -o $@ $< -L.. -llmclient -llmserver -llmcommon

clean:
	rm -f $(OBJS) *~
