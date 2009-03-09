BASEDIR = ..
include ../Makefile.in
OBJS = #put .o filenames here (DO NOT PUT .cpp!)

all: lmserver

lmserver: ../liblmserver.a main.o
	$(CXX) $(LDFLAGS) -o lmserver -L.. -llmserver -llmcommon main.o

../liblmserver.a: $(OBJ)
	$(AR) crus liblmserver.a $(OBJ)

clean:
	rm -f $(OBJ) main.o *~
