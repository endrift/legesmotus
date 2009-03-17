BASEDIR = ..
include ../Makefile.in
OBJS = simple_sprite

all: $(OBJS)

%: %.cpp ../liblmclient.a ../liblmcommon.a ../liblmserver.a
	$(CXX) $(LDFLAGS) $(CLIENTLIBS) -o $@ $< -L.. -llmcommon -llmclient -llmserver

clean:
	rm -f $(OBJS) *~
