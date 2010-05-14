BASEDIR = ..
OBJS = primitives.o
include ../common.mak
# Stopgap measure until client separated out
CXXFLAGS += -I$(BASEDIR)/client

all: $(BASEDIR)/liblmgui.a

$(BASEDIR)/liblmgui.a: $(OBJS)
	$(AR) crus $(BASEDIR)/liblmgui.a $(OBJS)

clean: common-clean
	$(RM) $(OBJS) $(BASEDIR)/liblmgui.a

deps: common-deps
