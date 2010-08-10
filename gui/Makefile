BASEDIR = ..
LIBSRCS := Widget.cpp DrawContext.cpp GLESContext.cpp GuiWindow.cpp Font.cpp Label.cpp Image.cpp ResourceCache.cpp \
	GraphicContainer.cpp Graphic.cpp Sprite.cpp Bone.cpp GraphicRegion.cpp Window.cpp SDLWindow.cpp SDLController.cpp
LIBRARY := ../liblmgui.a

include $(BASEDIR)/common.mk

# Stopgap measure until client separated out
CXXFLAGS += $(CLIENTFLAGS) $(shell freetype-config --cflags)
CXXFLAGS += -I../client

all: $(LIBRARY)

clean: common-clean
	@$(RM) $(LIBRARY)

deps: common-deps
