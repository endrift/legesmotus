BASEDIR = ..
OBJS = Controller.o GameLogic.o
include $(BASEDIR)/common.mk
CXXFLAGS += $(CLIENTFLAGS) $(shell freetype-config --cflags)

LIBRARY=../liblmnewclient.a

all: $(LIBRARY)

$(LIBRARY): $(OBJS)
	$(AR) crus $(LIBRARY) $^

clean: common-clean
	@$(RM) $(LIBRARY)

deps: common-deps
