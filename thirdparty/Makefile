BASEDIR = ..
LIBSRCS := simpleini/ConvertUTF.c
LIBRARY := ../libthirdparty.a

include $(BASEDIR)/common.mk

$(LIBOBJS):
	mkdir -p $(dir $(LIBOBJS))

all: mkdir $(LIBRARY)

.PHONY: mkdir
