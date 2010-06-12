BASEDIR = ..
OBJS = Widget.o DrawContext.o GLESContext.o GuiWindow.o Font.o Label.o
include $(BASEDIR)/common.mk
# Stopgap measure until client separated out
CXXFLAGS += $(CLIENTFLAGS)
CXXFLAGS += -I../client

all: ../liblmgui.a

../liblmgui.a: $(OBJS)
	$(AR) crus ../liblmgui.a $^

clean: common-clean
	@$(RM) ../liblmgui.a

deps: common-deps
