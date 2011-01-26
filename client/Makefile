BASEDIR = ..
LIBSRCS := Curve.cpp Controller.cpp Client.cpp ClientNetwork.cpp

include $(BASEDIR)/common.mk
LIBRARY := ../liblmclient.a

#ifeq ($(NOSOUND),)
#LIBSRCS += SoundController.cpp
#else
#CXXFLAGS += -DLM_NOSOUND
#LIBSRCS += SoundController_dummy.cpp
#endif

# XXX having to include this bit again is hackish...
$(LIBRARY): $(LIBOBJS)
	$(AR) crus $(LIBRARY) $^

#ifeq ($(MACHINE),Windows)
#BINSRCS += legesmotus.rc
#legesmotus.rc: legesmotus.ico
#endif
#ifeq ($(MACHINE),Darwin)
#BINSRCS += mac.m
#endif

all: $(LIBRARY)

clean: common-clean
	@$(RM) $(LIBRARY)

deps: common-deps
