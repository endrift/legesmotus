BASEDIR = ..
LIBSRCS := ClientNetwork.cpp ClientSDL.cpp GameController.cpp GameWindow.cpp Sprite.cpp GraphicalPlayer.cpp \
	Graphic.cpp TiledGraphic.cpp GraphicGroup.cpp GraphicalMap.cpp Font.cpp TextManager.cpp Text.cpp Mask.cpp \
	Radar.cpp TableBackground.cpp ScrollArea.cpp ScrollBar.cpp Transition.cpp Curve.cpp TransitionManager.cpp \
	ClientConfiguration.cpp MapReceiver.cpp ServerBrowser.cpp ChatLog.cpp RadialBackground.cpp \
	ConvolveKernel.cpp MapObjectParams.cpp BaseMapObject.cpp Obstacle.cpp Decoration.cpp ClientGate.cpp \
	ClientMap.cpp RepulsionField.cpp Menu.cpp MenuItem.cpp TextMenuItem.cpp ArbitraryMenu.cpp ForceField.cpp \
	Weapon.cpp StandardGun.cpp SpreadGun.cpp ImpactCannon.cpp Hazard.cpp Form.cpp FormItem.cpp ListMenuItem.cpp \
	RadialMenu.cpp GraphicMenuItem.cpp TextInput.cpp GraphicsCache.cpp ThawGun.cpp AreaGun.cpp ChargeGun.cpp \
	PenetrationGun.cpp
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
endif
ifeq ($(MACHINE),Darwin)
BINSRCS += mac.m
endif

all: legesmotus

legesmotus: $(BINOBJS) $(LIBRARY) ../liblmcommon.a
	$(CXX) $(LDFLAGS) -o legesmotus $^ $(CLIENTLIBS)

clean: common-clean
	@$(RM) $(LIBRARY) legesmotus

deps: common-deps
