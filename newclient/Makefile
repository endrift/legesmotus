BASEDIR = ..
LIBSRCS = Controller.cpp GameLogic.cpp
LIBRARY=../liblmnewclient.a

include $(BASEDIR)/common.mk

CXXFLAGS += $(CLIENTFLAGS) $(shell freetype-config --cflags)

all: $(LIBRARY)

clean: common-clean
	@$(RM) $(LIBRARY)

deps: common-deps
