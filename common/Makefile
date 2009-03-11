BASEDIR = ..
include ../Makefile.in
OBJS = LMException.o #put .o filenames here (DO NOT PUT .cpp!)

all: ../liblmcommon.a

../liblmcommon.a: $(OBJS)
	$(AR) crus ../liblmcommon.a $(OBJS)

clean:
	rm -f $(OBJS) *~
