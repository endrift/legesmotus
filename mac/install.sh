ln -sf /Applications/Leges\ Motus.app/Contents/MacOS/lmserver /usr/bin/lmserver
ln -sf /Applications/Leges\ Motus.app/Contents/MacOS/lmscan /usr/bin/lmscan
if [ x$legesmotus6 != x ]; then
	cp -f $legesmotus6 /usr/share/man/man6
fi
if [ x$lmserver6 != x ]; then
	cp -f $lmserver6 /usr/share/man/man6
fi
if [ x$lmscan6 != x ]; then
	cp -f $lmscan6 /usr/share/man/man6
fi
