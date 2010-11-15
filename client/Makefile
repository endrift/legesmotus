BASEDIR = ..
LIBSRCS := GraphicalPlayer.cpp GraphicalMap.cpp Transition.cpp Curve.cpp TransitionManager.cpp \
	ClientConfiguration.cpp MapReceiver.cpp ConvolveKernel.cpp MapObjectParams.cpp BaseMapObject.cpp ClientGate.cpp \
	ClientMap.cpp RepulsionField.cpp ForceField.cpp Weapon.cpp StandardGun.cpp SpreadGun.cpp ImpactCannon.cpp \
	ThawGun.cpp AreaGun.cpp ChargeGun.cpp PenetrationGun.cpp
BINSRCS := main.cpp

include $(BASEDIR)/common.mk
LIBRARY := ../liblmclient.a

CXXFLAGS += $(CLIENTFLAGS)

ifeq ($(NOSOUND),)
LIBSRCS += SoundController.cpp
else
CXXFLAGS += -DLM_NOSOUND
LIBSRCS += SoundController_dummy.cpp
endif

# XXX having to include this bit again is hackish...
$(LIBRARY): $(LIBOBJS)
	$(AR) crus $(LIBRARY) $^

ifeq ($(MACHINE),Windows)
BINSRCS += legesmotus.rc
legesmotus.rc: legesmotus.ico
endif
ifeq ($(MACHINE),Darwin)
BINSRCS += mac.m
endif

all: $(LIBRARY)

legesmotus: $(BINOBJS) $(LIBRARY) ../liblmcommon.a
	$(CXX) $(LDFLAGS) -o legesmotus $^ $(CLIENTLIBS)

clean: common-clean
	@$(RM) $(LIBRARY) legesmotus

deps: common-deps
