BASEDIR = .
include Makefile.in

ifeq ($(MACHINE)$(UNIXSTYLE),Darwin)
MACBUNDLE = bundle
endif

all: legesmotus $(MACBUNDLE)

legesmotus: common server client

common:
	$(MAKE) -C common

server: common
	$(MAKE) -C server

client: common
	$(MAKE) -C client

metaserver: common
	$(MAKE) -C metaserver

tests: common server client
	$(MAKE) -C tests

clean:
	$(MAKE) -C common clean
	$(MAKE) -C server clean
	$(MAKE) -C client clean
	$(MAKE) -C tests clean
	rm -rf "Leges Motus.app"


ifeq ($(MACHINE)$(UNIXSTYLE),Darwin)
Leges\ Motus.app: client server
	mkdir -p "Leges Motus.app/Contents/MacOS"
	mkdir -p "Leges Motus.app/Contents/Resources"
	mkdir -p "Leges Motus.app/Contents/Frameworks"
	cp -f client/legesmotus "Leges Motus.app/Contents/MacOS/"
	cp -f server/lmserver "Leges Motus.app/Contents/MacOS"
	cp -f client/Info.plist "Leges Motus.app/Contents/"
	cp -Rf client/legesmotus.nib "Leges Motus.app/Contents/Resources/"
	cp -Rf data "Leges Motus.app/Contents/Resources/"
	test -d "Leges Motus.app/Contents/Frameworks/SDL.framework" || cp -Rf /Library/Frameworks/SDL.framework "Leges Motus.app/Contents/Frameworks"
	test -d "Leges Motus.app/Contents/Frameworks/SDL_net.framework" || cp -Rf /Library/Frameworks/SDL_net.framework "Leges Motus.app/Contents/Frameworks"
	test -d "Leges Motus.app/Contents/Frameworks/SDL_image.framework" || cp -Rf /Library/Frameworks/SDL_image.framework "Leges Motus.app/Contents/Frameworks"
	test -d "Leges Motus.app/Contents/Frameworks/SDL_ttf.framework" || cp -Rf /Library/Frameworks/SDL_ttf.framework "Leges Motus.app/Contents/Frameworks"
	test -d "Leges Motus.app/Contents/Frameworks/SDL_mixer.framework" || cp -Rf /Library/Frameworks/SDL_mixer.framework "Leges Motus.app/Contents/Frameworks"

bundle: Leges\ Motus.app

install:
	cp -Rf Leges\ Motus.app /Applications/
	ln -s /Applications/Leges\ Motus.app/Contents/MacOS/lmserver $(BINDIR)

uninstall:
	rm -rf /Applications/Leges\ Motus.app
	rm -f $(BINDIR)/lmserver
else

ifneq ($(PREFIX),)
install:
	install -d $(DATADIR)/fonts
	install -m 0644 $(BASEDIR)/data/fonts/* $(DATADIR)/fonts
	install -d $(DATADIR)/maps
	install -m 0644 $(BASEDIR)/data/maps/* $(DATADIR)/maps
	install -d $(DATADIR)/sounds
	install -m 0644 $(BASEDIR)/data/sounds/* $(DATADIR)/sounds
	install -d $(DATADIR)/sprites
	install -m 0644 $(BASEDIR)/data/sprites/* $(DATADIR)/sprites
	install -d $(BINDIR)
	install -s $(BASEDIR)/server/lmserver $(BASEDIR)/client/legesmotus $(BINDIR)

uninstall:
	rm -rf $(DATADIR)
	rm -f $(BINDIR)/lmserver $(BINDIR)/legesmotus
endif

endif

.PHONY: clean common server client bundle metaserver install uninstall
