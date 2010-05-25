default:

all: default

BASEDIR = $(shell echo $(realpath .) | sed -e 's/ /\\ /')
include $(BASEDIR)/common.mk

distclean: clean
	$(RM) config.mk

ifeq ($(BUILDDIR),)

ifneq ($(ARCH),)
MACHINE_TARGETS = build/$(MACHINE)-$(ARCH)
else
MACHINE_TARGETS = $(foreach ARCH,$(ARCHS),build/$(MACHINE)-$(ARCH))
endif
isolate-arch = $(patsubst build/$(MACHINE)-%,%,$(1))

%:: $(MACHINE_TARGETS)

default:
default: build $(MACHINE_TARGETS)

$(MACHINE_TARGETS):
	mkdir -p $@
	$(MAKE) BUILDDIR=$@ ARCH=$(call isolate-arch,$@) $(MAKECMDGOALS)

UNIVERSAL_TARGET=build/$(MACHINE)-universal
ifeq ($(MACHINE)$(UNIXSTYLE),Darwin)
bundle: TARGET = bundle
bundle: $(MACHINE_TARGETS)

ifneq ($(UNIVERSAL),)
bundle:
	mkdir -p $(UNIVERSAL_TARGET)
	cp -r $(firstword $(MACHINE_TARGETS))/Leges\ Motus.app $(UNIVERSAL_TARGET)/
	lipo -create $(foreach TARGET,$(MACHINE_TARGETS),-arch $(call isolate-arch,$(TARGET)) $(TARGET)/Leges\ Motus.app/Contents/MacOS/legesmotus) -output $(UNIVERSAL_TARGET)/Leges\ Motus.app/Contents/MacOS/legesmotus
	lipo -create $(foreach TARGET,$(MACHINE_TARGETS),-arch $(call isolate-arch,$(TARGET)) $(TARGET)/Leges\ Motus.app/Contents/MacOS/lmserver) -output $(UNIVERSAL_TARGET)/Leges\ Motus.app/Contents/MacOS/lmserver
endif
endif

build:
build:
	mkdir -p build

clean:
clean:
	$(RM) -r build

.PHONY: default clean $(MACHINE_TARGETS)

else

INSTALL_TARGETS = $(addprefix install-,$(TARGETS))
SRC_PKG = common server client gui
AUX_PKG = tests serverscanner metaserver
ALL_PKG = $(SRC_PKG) $(AUX_PKG)

default: $(TARGETS)

legesmotus: client

lmserver: server

$(ALL_PKG):
	@mkdir -p $(BUILDDIR)/$@
	$(MAKE) -C $(BUILDDIR)/$@ -f $(BASEDIR)/$@/Makefile BASEDIR="../../.." SUBDIR="$@"

server: common

client: common

gui: common client

serverscanner: common client

metaserver: common

tests: common server client gui

clean:
	$(RM) -r $(BUILDDIR)

deps:
	$(MAKE) -C common deps
	$(MAKE) -C server deps
	$(MAKE) -C client deps

ifeq ($(MACHINE)$(UNIXSTYLE),Darwin)

CLI_INSTALLER = Install\ Command\ Line\ Tools.app
DMG = legesmotus-$(VERSION)-mac.dmg
EXTRA_PHONY = bundle package cli-installer

$(BUILDDIR)/Leges\ Motus.app: client server
	mkdir -p "$@"/Contents/MacOS
	mkdir -p "$@"/Contents/Resources
	mkdir -p "$@"/Contents/Frameworks
	cp -f $(BUILDDIR)/client/legesmotus "$@"/Contents/MacOS/
	cp -f $(BUILDDIR)/server/lmserver "$@"/Contents/MacOS
	cp -f client/Info.plist "$@"/Contents/
	cp -f client/legesmotus.icns "$@"/Contents/Resources/
	cp -Rf client/legesmotus.nib "$@"/Contents/Resources/
	cp -Rf data "$@"/Contents/Resources/
	sed -e 's/\$$VERSION/$(VERSION)/' -i '' "$@"/Contents/Info.plist
	find "$@" -name .svn -print0 | xargs -0 rm -rf
	test -d "$@"/Contents/Frameworks/SDL.framework || cp -Rf /Library/Frameworks/SDL.framework "$@"/Contents/Frameworks
	test -d "$@"/Contents/Frameworks/SDL_image.framework || cp -Rf /Library/Frameworks/SDL_image.framework "$@"/Contents/Frameworks
	test -d "$@"/Contents/Frameworks/SDL_ttf.framework || cp -Rf /Library/Frameworks/SDL_ttf.framework "$@"/Contents/Frameworks
	test -d "$@"/Contents/Frameworks/SDL_mixer.framework || cp -Rf /Library/Frameworks/SDL_mixer.framework "$@"/Contents/Frameworks

$(DMG): bundle $(CLI_INSTALLER)
	$(RM) -r tmp
	mkdir -p tmp
	cp client/legesmotus.icns tmp/.VolumeIcon.icns
	SetFile -c icnC tmp/.VolumeIcon.icns
	cp README tmp/README.TXT
	cp CHANGES COPYING NEW_MAP_FORMAT tmp
	cp -R Leges\ Motus.app tmp
	cp -R $(CLI_INSTALLER) tmp
	hdiutil create -srcfolder tmp -volname "Leges Motus $(VERSION)" -format UDRW -ov raw-$(DMG)
	$(RM) -r tmp
	mkdir -p tmp
	# This is to set the volume icon. No, really
	hdiutil attach raw-$(DMG) -mountpoint tmp
	SetFile -a C tmp
	hdiutil detach tmp
	$(RM) $(DMG)
	hdiutil convert raw-$(DMG) -format UDZO -o $(DMG)
	$(RM) raw-$(DMG)
	$(RM) -r tmp
	

$(CLI_INSTALLER): mac/install.applescript mac/install.sh
	osacompile -o $(CLI_INSTALLER) mac/install.applescript
	cp -f man/man6/* $(CLI_INSTALLER)/Contents/Resources
	cp -f mac/install.sh $(CLI_INSTALLER)/Contents/Resources

bundle: $(BUILDDIR)/Leges\ Motus.app

package: $(DMG)

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

EXTRA_PHONY = install-client install-server install-common

install: default $(INSTALL_TARGETS)

install-client: client install-common
	install -d $(DESTDIR)$(DATADIR)/fonts
	install -m 0644 data/fonts/* $(DESTDIR)$(DATADIR)/fonts
	install -d $(DESTDIR)$(DATADIR)/sounds
	install -m 0644 data/sounds/* $(DESTDIR)$(DATADIR)/sounds
	install -d $(DESTDIR)$(DATADIR)/sprites
	install -m 0644 data/sprites/* $(DESTDIR)$(DATADIR)/sprites
	install -d $(DESTDIR)$(MANDIR)/man6
	install -m 0644 man/man6/legesmotus.6 $(DESTDIR)$(MANDIR)/man6
	install -d $(DESTDIR)$(SHAREDIR)/applications
	install -m 0644 client/legesmotus.desktop $(DESTDIR)$(SHAREDIR)/applications/legesmotus.desktop
	sed -e 's/\$$VERSION/$(subst /,\/,$(VERSION))/' $(INPLACE) $(DESTDIR)$(SHAREDIR)/applications/legesmotus.desktop
	sed -e 's/\$$SHAREDIR/$(subst /,\/,$(SHAREDIR))/' $(INPLACE) $(DESTDIR)$(SHAREDIR)/applications/legesmotus.desktop
	sed -e 's/\$$BINDIR/$(subst /,\/,$(BINDIR))/' $(INPLACE) $(DESTDIR)$(SHAREDIR)/applications/legesmotus.desktop
	install -d $(DESTDIR)$(SHAREDIR)/icons/hicolor/256x256
	install -m 0644 data/sprites/blue_head256.png $(DESTDIR)$(SHAREDIR)/icons/hicolor/256x256/legesmotus.png
	which update-desktop-database && update-desktop-database $(DESTDIR)$(SHAREDIR)/applications || true
	install -d $(DESTDIR)$(BINDIR)
	install $(BUILD)/client/legesmotus $(DESTDIR)$(BINDIR)
	strip $(DESTDIR)$(BINDIR)/legesmotus
	
install-server: server install-common
	install -d $(DESTDIR)$(MANDIR)/man6
	install -m 0644 man/man6/lmserver.6 $(DESTDIR)$(MANDIR)/man6
	install -d $(DESTDIR)$(BINDIR)
	install $(BUILD)/server/lmserver $(DESTDIR)$(BINDIR)
	strip $(DESTDIR)$(BINDIR)/lmserver

install-common:
	install -d $(DESTDIR)$(DATADIR)/maps
	install -m 0644 data/maps/* $(DESTDIR)$(DATADIR)/maps
	install -d $(DESTDIR)$(DATADIR)/weapons
	install -m 0644 data/weapons/* $(DESTDIR)$(DATADIR)/weapons

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

.PHONY: deps clean common server client metaserver default install uninstall $(EXTRA_PHONY) $(ALL_PKGS) $(PKG_DIRS)

endif
