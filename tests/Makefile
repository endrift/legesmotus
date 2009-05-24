BASEDIR = ..
include ../Makefile.in
OBJS = client_network simple_sprite simple_text timer

all: $(OBJS)

%: %.cpp ../liblmclient.a ../liblmcommon.a ../liblmserver.a
	$(CXX) $(LDFLAGS) $(CLIENTLIBS) -o $@ $< -L.. -llmclient -llmserver -llmcommon

clean:
	rm -f $(OBJS) *~
