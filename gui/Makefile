BASEDIR = ..
OBJS = Widget.o DrawContext.o GLESContext.o GuiWindow.o Font.o Label.o \
	Image.o ResourceCache.o GraphicContainer.o Graphic.o Sprite.o Bone.o \
	GraphicRegion.o Window.o SDLWindow.o SDLController.o
include $(BASEDIR)/common.mk
# Stopgap measure until client separated out
CXXFLAGS += $(CLIENTFLAGS) $(shell freetype-config --cflags)
CXXFLAGS += -I../client

all: ../liblmgui.a

../liblmgui.a: $(OBJS)
	$(AR) crus ../liblmgui.a $^

clean: common-clean
	@$(RM) ../liblmgui.a

deps: common-deps
