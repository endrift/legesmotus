BASEDIR = ..
OBJS = primitives.o
include ../common.mak
# Stopgap measure until client separated out
CXXFLAGS += -I../client

all: ../liblmgui.a

../liblmgui.a: $(OBJS)
	$(AR) crus ../liblmgui.a $(OBJS)

clean: common-clean
	$(RM) $(OBJS) ../liblmgui.a

deps: common-deps
