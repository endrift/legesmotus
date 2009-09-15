legesmotus:
BASEDIR = .
include common.mak

ifeq ($(MACHINE)$(UNIXSTYLE),Darwin)
MACBUNDLE = bundle
endif

all: deps legesmotus $(MACBUNDLE)

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
	$(MAKE) -C metaserver clean
	$(MAKE) -C tests clean
	$(RM) -r "Leges Motus.app"
	$(RM) -r legesmotus*.pkg
	$(RM) README.rtf

deps:
	$(MAKE) -C common deps
	$(MAKE) -C server deps
	$(MAKE) -C client deps
	$(MAKE) -C metaserver deps

README.rtf: README
	echo '{\\rtf1\\ansi' > README.rtf # Header
	echo '{\\fonttbl\\f0 Courier;}' >> README.rtf # Set font to Courier
	echo '\\f0\\fs20' >> README.rtf # Set font size to 10pt
	echo '\\tx960\\tx1920\\tx2880\\tx3840\\tx4800\\tx5760\\tx6720\\tx7680\\tx8640\\tx9600' >> README.rtf # Set tabstop to 8
	cat README | sed -e 's/\\/\\\\/g' -e 's/$$/\\/g' >> README.rtf # Parse file
	echo '}' >> README.rtf # Footer

ifeq ($(MACHINE)$(UNIXSTYLE),Darwin)

CLI_INSTALLER = Install\ Command\ Line\ Tools.app

Leges\ Motus.app: client server
	mkdir -p "Leges Motus.app/Contents/MacOS"
	mkdir -p "Leges Motus.app/Contents/Resources"
	mkdir -p "Leges Motus.app/Contents/Frameworks"
	cp -f client/legesmotus "Leges Motus.app/Contents/MacOS/"
	cp -f server/lmserver "Leges Motus.app/Contents/MacOS"
	cp -f client/Info.plist "Leges Motus.app/Contents/"
	cp -f client/legesmotus.icns "Leges Motus.app/Contents/Resources/"
	cp -Rf client/legesmotus.nib "Leges Motus.app/Contents/Resources/"
	cp -Rf data "Leges Motus.app/Contents/Resources/"
	sed -e 's/\$$VERSION/$(VERSION)/' -i '' "Leges Motus.app/Contents/Info.plist"
	find "Leges Motus.app" -name .svn -print0 | xargs -0 rm -rf
	test -d "Leges Motus.app/Contents/Frameworks/SDL.framework" || cp -Rf /Library/Frameworks/SDL.framework "Leges Motus.app/Contents/Frameworks"
	test -d "Leges Motus.app/Contents/Frameworks/SDL_net.framework" || cp -Rf /Library/Frameworks/SDL_net.framework "Leges Motus.app/Contents/Frameworks"
	test -d "Leges Motus.app/Contents/Frameworks/SDL_image.framework" || cp -Rf /Library/Frameworks/SDL_image.framework "Leges Motus.app/Contents/Frameworks"
	test -d "Leges Motus.app/Contents/Frameworks/SDL_ttf.framework" || cp -Rf /Library/Frameworks/SDL_ttf.framework "Leges Motus.app/Contents/Frameworks"
	test -d "Leges Motus.app/Contents/Frameworks/SDL_mixer.framework" || cp -Rf /Library/Frameworks/SDL_mixer.framework "Leges Motus.app/Contents/Frameworks"

legesmotus-$(VERSION).pkg: bundle README.rtf
	rm -Rf tmp
	mkdir -p tmp/Leges\ Motus/Applications
	mkdir -p tmp/Leges\ Motus/usr/bin
	mkdir -p tmp/Leges\ Motus/usr/share/man/man6
	mkdir -p tmp/Resources/en.lproj
	cp -Rf Leges\ Motus.app tmp/Leges\ Motus/Applications/
	cp -Rf man/man6/* tmp/Leges\ Motus/usr/share/man/man6/
	cd tmp/Leges\ Motus/usr/bin && \
		ln -sf ../../Applications/Leges\ Motus.app/Contents/MacOS/lmserver
	cp -f README.rtf tmp/Resources/en.lproj/ReadMe.rtf
	cp -f COPYING tmp/Resources/en.lproj/License.txt
	/Developer/usr/bin/packagemaker --root tmp/Leges\ Motus --id org.legesmotus.legesmotus \
		--title "Leges Motus $(VERSION)" --version "$(VERSION)" --resources tmp/Resources \
		--target 10.4 --root-volume-only --out legesmotus-$(VERSION).pkg
	# The following two lines are a hack to prevent the installer from thinking the packages are relocatable
	$(RM) legesmotus-$(VERSION).pkg/Contents/Resources/TokenDefinitions.plist
	defaults delete "`pwd`/legesmotus-$(VERSION).pkg/Contents/Info" IFPkgPathMappings
	$(RM) -r tmp

$(CLI_INSTALLER): mac/install.applescript mac/install.sh
	osacompile -o $(CLI_INSTALLER) mac/install.applescript
	cp -f man/man6/* $(CLI_INSTALLER)/Contents/Resources
	cp -f mac/install.sh $(CLI_INSTALLER)/Contents/Resources

bundle: Leges\ Motus.app

package: legesmotus-$(VERSION).pkg

cli-installer: $(CLI_INSTALLER)

install:
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
install:
	install -d $(DESTDIR)$(DATADIR)/fonts
	install -m 0644 $(BASEDIR)/data/fonts/* $(DESTDIR)$(DATADIR)/fonts
	install -d $(DESTDIR)$(DATADIR)/maps
	install -m 0644 $(BASEDIR)/data/maps/* $(DESTDIR)$(DATADIR)/maps
	install -d $(DESTDIR)$(DATADIR)/sounds
	install -m 0644 $(BASEDIR)/data/sounds/* $(DESTDIR)$(DATADIR)/sounds
	install -d $(DESTDIR)$(DATADIR)/sprites
	install -m 0644 $(BASEDIR)/data/sprites/* $(DESTDIR)$(DATADIR)/sprites
	install -d $(DESTDIR)$(MANDIR)/man6
	install -m 0644 $(BASEDIR)/man/man6/* $(DESTDIR)$(MANDIR)/man6
	install -d $(DESTDIR)$(MANDIR)/man6
	install -m 0644 $(BASEDIR)/man/man6/* $(DESTDIR)$(MANDIR)/man6
	install -d $(DESTDIR)$(PREFIX)/share/applications
	install -m 0644 $(BASEDIR)/client/legesmotus.desktop $(DESTDIR)$(PREFIX)/share/applications/legesmotus.desktop
	sed -e 's/\$$VERSION/$(subst /,\/,$(VERSION))/' -i'' $(DESTDIR)$(PREFIX)/share/applications/legesmotus.desktop
	sed -e 's/\$$DESTDIR/$(subst /,\/,DESTDIR))/' -i'' $(DESTDIR)$(PREFIX)/share/applications/legesmotus.desktop
	sed -e 's/\$$PREFIX/$(subst /,\/,PREFIX))/' -i'' $(DESTDIR)$(PREFIX)/share/applications/legesmotus.desktop
	sed -e 's/\$$BINDIR/$(subst /,\/,BINDIR))/' -i'' $(DESTDIR)$(PREFIX)/share/applications/legesmotus.desktop
	install -d $(DESTDIR)$(PREFIX)/share/icons/hicolor/256x256
	install -m 0644 $(BASEDIR)/data/sprites/blue_head256.png $(DESTDIR)$(PREFIX)/share/icons/hicolor/256x256/legesmotus.png
	install -d $(DESTDIR)$(BINDIR)
	where update-desktop-database && update-desktop-database
	install $(BASEDIR)/server/lmserver $(BASEDIR)/client/legesmotus $(DESTDIR)$(BINDIR)
	strip $(DESTDIR)$(BINDIR)/legesmotus
	strip $(DESTDIR)$(BINDIR)/lmserver

uninstall:
	$(RM) -r $(DESTDIR)$(DATADIR)
	$(RM) $(DESTDIR)$(BINDIR)/lmserver $(DESTDIR)$(BINDIR)/legesmotus
	$(RM) $(DESTDIR)$(MANDIR)/man6/lmserver.6 $(DESTDIR)$(MANDIR)/man6/legesmotus.6
	$(RM) $(DESTDIR)$(PREFIX)/share/icons/hicolor/256x256/legesmotus.png $(DESTDIR)$(PREFIX)/share/applications/legesmotus.desktop
else
$(error Prefix not specified. Please specify one by running the configure script)
endif
endif

endif

cscope: 
	find . -follow -name SCCS -prune -o -name '*.[ch]pp' -print | grep -v svn > cscope.files
	cscope -v -q

.PHONY: clean common server client bundle package metaserver install uninstall
