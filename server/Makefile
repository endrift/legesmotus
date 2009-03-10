BASEDIR = ..
include ../Makefile.in
OBJS = #put .o filenames here (DO NOT PUT .cpp!)

INCLUDES = -I..

CXXFLAGS += $(INCLUDES)

all: lmserver

lmserver: ../liblmserver.a ../liblmcommon.a main.o
	$(CXX) $(LDFLAGS) -o lmserver -L.. main.o -llmserver -llmcommon

../liblmserver.a: $(OBJS)
	$(AR) crus ../liblmserver.a $(OBJS)

clean:
	rm -f $(OBJS) main.o *~
