default:

all: default

BASEDIR = $(shell echo $(realpath .) | sed -e 's/ /\\ /')
include $(BASEDIR)/common.mk

distclean:
	$(RM) -r build
	$(RM) config.mk

.PHONY: distclean default all

ifeq ($(ARCH),)

# Figure out chdir

MACHINE_TARGETS = $(foreach ARCH,$(ARCHS),$(MACHINE)-$(ARCH)-$(RELEASE))
isolate-arch = $(patsubst $(MACHINE)-%-$(RELEASE),%,$(1))
UNIVERSAL_TARGET = $(MACHINE)-universal-$(RELEASE)
BUILDROOT = build
NOCHAIN = dist

ifneq ($(DESTDIR),)
 # Can't seem to pass overridden variables
 PASSED = DESTDIR="$(abspath $(DESTDIR))"
endif

CHAINED_GOALS= $(filter $(MAKECMDGOALS),$(NOCHAIN))
ifeq ($(CHAINED_GOALS),)
# This magic target lets us chain targets we can't find to the other targets
%: $(MACHINE_TARGETS)
	@true # Dummy command prevents "nothing to be done"

%.mk:
Makefile:

else
 ifneq ($(CHAINED_GOALS),$(MAKECMDGOALS))
 $(error Cannot build these goals at the same time as the rest: $(CHAINGED_GOALS))
 endif
endif

default: $(MACHINE_TARGETS)
	@true # Dummy command prevents "nothing to be done"

$(MACHINE_TARGETS): TARGET=$(MAKECMDGOALS)
$(MACHINE_TARGETS) $(UNIVERSAL_TARGET):
	+@mkdir -p $(BUILDROOT)/$@
	+$(MAKE) -C $(BUILDROOT)/$@ -f $(BASEDIR)/Makefile BASEDIR="$(BASEDIR)" ARCH=$(call isolate-arch,$@) $(PASSED) $(TARGET)

ifeq ($(MACHINE)$(NOBUNDLE),Darwin)
bundle: TARGET = bundle
bundle: $(MACHINE_TARGETS)

ifneq ($(UNIVERSAL),)
bundle:
	mkdir -p $(BUILDROOT)/$(UNIVERSAL_TARGET)
	cp -r $(BUILDROOT)/$(firstword $(MACHINE_TARGETS))/Leges\ Motus.app $(BUILDROOT)/$(UNIVERSAL_TARGET)/
	lipo -create $(foreach TARGET,$(MACHINE_TARGETS),-arch $(call isolate-arch,$(TARGET)) $(BUILDROOT)/$(TARGET)/Leges\ Motus.app/Contents/MacOS/legesmotus ) -output $(BUILDROOT)/$(UNIVERSAL_TARGET)/Leges\ Motus.app/Contents/MacOS/legesmotus
	lipo -create $(foreach TARGET,$(MACHINE_TARGETS),-arch $(call isolate-arch,$(TARGET)) $(BUILDROOT)/$(TARGET)/Leges\ Motus.app/Contents/MacOS/lmserver ) -output $(BUILDROOT)/$(UNIVERSAL_TARGET)/Leges\ Motus.app/Contents/MacOS/lmserver

dist: TARGET=cli-installer dist
dist: bundle $(UNIVERSAL_TARGET)

else
dist: bundle $(MACHINE_TARGETS)
endif

endif

.PHONY: default $(MACHINE_TARGETS)

else

# We are in our chdir

INSTALL_TARGETS = $(addprefix install-,$(TARGETS))

ifneq ($(ARCH),universal)

SRC_PKG = common server client gui
AUX_PKG = tests serverscanner metaserver
ALL_PKG = $(SRC_PKG) $(AUX_PKG)

default: $(TARGETS)

legesmotus: client

lmserver: server

$(ALL_PKG):
	+@mkdir -p $@
	+@$(MAKE) -C $@ -f $(BASEDIR)/$@/Makefile BASEDIR="../../.." SUBDIR="$@" $(TARGET)

server: common

client: common

gui: common client

serverscanner: common

metaserver: common

tests: common server client gui

else
default:
	$(error Must specify a target when doing a universal sub-build)
endif

clean: TARGET=clean
clean: common-clean $(ALL_PKG)

tidy: TARGET=tidy
tidy: common-tidy $(ALL_PKG)

deps:
	$(MAKE) -C common deps
	$(MAKE) -C server deps
	$(MAKE) -C client deps

ifeq ($(MACHINE)$(NOBUNDLE),Darwin)

CLI_INSTALLER = Install\ Command\ Line\ Tools.app
DMG = legesmotus-$(VERSION)-mac-$(ARCH).dmg
DMG_NAME = "Leges Motus $(VERSION)"
.PHONY: bundle dist cli-installer

ifneq ($(ARCH),universal)
Leges\ Motus.app: client server
	mkdir -p "$@"/Contents/MacOS
	mkdir -p "$@"/Contents/Resources
	mkdir -p "$@"/Contents/Frameworks
	cp -f client/legesmotus "$@"/Contents/MacOS/
	cp -f server/lmserver "$@"/Contents/MacOS
	cp -f $(BASEDIR)/client/Info.plist "$@"/Contents/
	cp -f $(BASEDIR)/client/legesmotus.icns "$@"/Contents/Resources/
	cp -Rf $(BASEDIR)/client/legesmotus.nib "$@"/Contents/Resources/
	cp -Rf $(BASEDIR)/data "$@"/Contents/Resources/
	sed -e 's/\$$VERSION/$(VERSION)/' -i '' "$@"/Contents/Info.plist
	find "$@" -name .svn -print0 | xargs -0 rm -rf
	test -d "$@"/Contents/Frameworks/SDL.framework || cp -Rf /Library/Frameworks/SDL.framework "$@"/Contents/Frameworks
	test -d "$@"/Contents/Frameworks/SDL_image.framework || cp -Rf /Library/Frameworks/SDL_image.framework "$@"/Contents/Frameworks
	test -d "$@"/Contents/Frameworks/SDL_ttf.framework || cp -Rf /Library/Frameworks/SDL_ttf.framework "$@"/Contents/Frameworks
	test -d "$@"/Contents/Frameworks/SDL_mixer.framework || cp -Rf /Library/Frameworks/SDL_mixer.framework "$@"/Contents/Frameworks

bundle: Leges\ Motus.app

$(DMG): bundle
endif

$(DMG): $(CLI_INSTALLER)
	$(RM) -r tmp
	mkdir -p tmp
	mkdir -p tmp/Other\ Stuff
	cp $(BASEDIR)/client/legesmotus.icns tmp/.VolumeIcon.icns
	SetFile -c icnC tmp/.VolumeIcon.icns
	cp $(BASEDIR)/README tmp/README.TXT
	cp $(BASEDIR)/{CHANGES,COPYING,NEW_MAP_FORMAT} tmp/Other\ Stuff
	cp -R Leges\ Motus.app tmp
	cp -R $(CLI_INSTALLER) tmp/Other\ Stuff
	cp $(BASEDIR)/mac/lmbg.png tmp/.lmbg.png
	hdiutil create -srcfolder tmp -volname $(DMG_NAME) -format UDRW -ov raw-$(DMG)
	$(RM) -r tmp
	mkdir -p tmp
	@# This is to set the volume icon. No, really
	hdiutil attach raw-$(DMG) -mountpoint tmp
	SetFile -a C tmp
	osascript $(BASEDIR)/mac/polish_dmg.applescript $(DMG_NAME)
	hdiutil detach tmp
	$(RM) $(DMG)
	hdiutil convert raw-$(DMG) -format UDZO -o $(DMG)
	#$(RM) raw-$(DMG)
	$(RM) -r tmp
	

$(CLI_INSTALLER):
	osacompile -o $(CLI_INSTALLER) $(BASEDIR)/mac/install.applescript
	cp -f $(BASEDIR)/man/man6/* $(CLI_INSTALLER)/Contents/Resources
	cp -f $(BASEDIR)/mac/install.sh $(CLI_INSTALLER)/Contents/Resources

dist: $(DMG)

cli-installer: $(CLI_INSTALLER)

install: bundle
	cp -Rf Leges\ Motus.app /Applications/
	ln -sf /Applications/Leges\ Motus.app/Contents/MacOS/lmserver $(BINDIR)
	install -d $(MANDIR)/man6
	install -m 0644 $(BASEDIR)/man/man6/legesmotus.6 $(MANDIR)/man6
	install -m 0644 $(BASEDIR)/man/man6/lmserver.6 $(MANDIR)/man6

uninstall:
	$(RM) -r /Applications/Leges\ Motus.app
	$(RM) $(BINDIR)/lmserver
	$(RM) $(MANDIR)/man6/lmserver.6
else

ifneq ($(MACHINE),Windows)
ifneq ($(PREFIX),)

.PHONY: install-client install-server install-common

ifneq ($(DESTDIR),)
 override DESTDIR := "$(DESTDIR)"/
endif

install: default $(INSTALL_TARGETS)

install-client: client install-common
	install -d $(DESTDIR)$(DATADIR)/fonts
	install -m 0644 $(BASEDIR)/data/fonts/* $(DESTDIR)$(DATADIR)/fonts
	install -d $(DESTDIR)$(DATADIR)/sounds
	install -m 0644 $(BASEDIR)/data/sounds/* $(DESTDIR)$(DATADIR)/sounds
	install -d $(DESTDIR)$(DATADIR)/sprites
	install -m 0644 $(BASEDIR)/data/sprites/* $(DESTDIR)$(DATADIR)/sprites
	install -d $(DESTDIR)$(MANDIR)/man6
	install -m 0644 $(BASEDIR)/man/man6/legesmotus.6 $(DESTDIR)$(MANDIR)/man6
	install -d $(DESTDIR)$(SHAREDIR)/applications
	install -m 0644 $(BASEDIR)/client/legesmotus.desktop $(DESTDIR)$(SHAREDIR)/applications/legesmotus.desktop
	sed -e 's/\$$VERSION/$(subst /,\/,$(VERSION))/' $(INPLACE) $(DESTDIR)$(SHAREDIR)/applications/legesmotus.desktop
	sed -e 's/\$$SHAREDIR/$(subst /,\/,$(SHAREDIR))/' $(INPLACE) $(DESTDIR)$(SHAREDIR)/applications/legesmotus.desktop
	sed -e 's/\$$BINDIR/$(subst /,\/,$(BINDIR))/' $(INPLACE) $(DESTDIR)$(SHAREDIR)/applications/legesmotus.desktop
	install -d $(DESTDIR)$(SHAREDIR)/icons/hicolor/256x256
	install -m 0644 $(BASEDIR)/data/sprites/blue_head256.png $(DESTDIR)$(SHAREDIR)/icons/hicolor/256x256/legesmotus.png
	which update-desktop-database && update-desktop-database $(DESTDIR)$(SHAREDIR)/applications || true
	install -d $(DESTDIR)$(BINDIR)
	install client/legesmotus $(DESTDIR)$(BINDIR)
	strip $(DESTDIR)$(BINDIR)/legesmotus
	
install-server: server install-common
	install -d $(DESTDIR)$(MANDIR)/man6
	install -m 0644 $(BASEDIR)/man/man6/lmserver.6 $(DESTDIR)$(MANDIR)/man6
	install -d $(DESTDIR)$(BINDIR)
	install server/lmserver $(DESTDIR)$(BINDIR)
	strip $(DESTDIR)$(BINDIR)/lmserver

install-common:
	install -d $(DESTDIR)$(DATADIR)/maps
	install -m 0644 $(BASEDIR)/data/maps/* $(DESTDIR)$(DATADIR)/maps
	install -d $(DESTDIR)$(DATADIR)/weapons
	install -m 0644 $(BASEDIR)/data/weapons/* $(DESTDIR)$(DATADIR)/weapons

uninstall:
	$(RM) -r $(DESTDIR)$(DATADIR)
	$(RM) $(DESTDIR)$(BINDIR)/lmserver $(DESTDIR)$(BINDIR)/legesmotus
	$(RM) $(DESTDIR)$(MANDIR)/man6/lmserver.6 $(DESTDIR)$(MANDIR)/man6/legesmotus.6
	$(RM) $(DESTDIR)$(PREFIX)/share/icons/hicolor/256x256/legesmotus.png $(DESTDIR)$(PREFIX)/share/applications/legesmotus.desktop
#else
#$(error Prefix not specified. Please specify one by running the configure script)
endif
endif

endif

cscope: 
	find . -follow -name SCCS -prune -o -name '*.[ch]pp' -print | grep -v svn > cscope.files
	cscope -v -q

.PHONY: deps clean common server client metaserver install uninstall $(ALL_PKGS) $(PKG_DIRS)

endif
