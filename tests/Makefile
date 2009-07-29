BASEDIR = ..
UNIXSTYLE = 1
include ../common.mak
OBJS = client_network config_test simple_sprite simple_text simple_scroll simple_transition \
	simple_menu

all: $(OBJS)

%: %.cpp ../liblmclient.a ../liblmcommon.a ../liblmserver.a
	$(CXX) $(CXXFLAGS) $(CLIENTFLAGS) $(CLIENTLIBS) -o $@ $< -L.. -llmclient -llmserver -llmcommon

clean:
	rm -f $(OBJS) *~
