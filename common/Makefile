BASEDIR = ..
include ../Makefile.in
OBJS = #put .o filenames here (DO NOT PUT .cpp!)

all: ../liblmcommon.a

../liblmcommon.a: $(OBJ)
	$(AR) crus liblmcommon.a $(OBJ)

clean:
	rm -f $(OBJ) *~
