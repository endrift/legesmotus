#!/bin/bash

clean_framework() {
	echo Cleaning $1 in $(pwd)

	rm -rf $1.framework/Headers
	#strip $1.framework/$1

	rm -rf $1.framework/Versions/*/Headers
	#strip $1.framework/Versions/*/$1

	for d in $1.framework/Frameworks/*; do
		if [ -d $d ]; then
			DIR=$(basename $d .framework)
			pushd $1.framework/Frameworks >/dev/null
			clean_framework $DIR
			popd >/dev/null
		fi
	done
}

if [ x$1 = x ]; then
	exit 1
fi

FRAMEWORK=$1
APP_BUNDLE=$2

cd "$APP_BUNDLE/Contents/Frameworks"

if [ ! -d $FRAMEWORK.framework ] ; then
	rm -f $FRAMEWORK.framework
	cp -Rf /Library/Frameworks/$FRAMEWORK.framework .
fi

echo Preparing $FRAMEWORK

clean_framework $FRAMEWORK
