#! /bin/bash

GIT_REPOSITORY=http://github.com/unclewerner/libftl.git
GIT_VERSION=testing # "stable" or "testing"
SOURCE_FOLDER=$PWD/libftl-src
TARGET_FOLDER=$PWD
CHECKOUT=1
CONFIGURE=1
CONFIGURE_CLEAN=1
BUILD=1
BUILD_STATIC=1
BUILD_SHARED=1
MAKEFLAGS= # e.g. -j2

if [ -e ./update-options.sh ]; then
	. ./update-options.sh
fi

function msg() { echo "$(basename $0): $1"; }
function debug() { echo "$(basename $0): $1" 1>&2; }

function err() {
	ret=$?
	if [ "$1" != "" ]; then echo "$(basename $0): $1" 1>&2; fi
	if [ $ret -ne 0 ]; then exit $ret; else exit 1; fi
}

function version_max() {
	let XM=0 YM=0 ZM=0
	for t in $(git tag); do
		IFS_SAVED=$IFS
		IFS='.-'
		let i=0
		for v in $t; do
			[ $i -eq 0 ] && let X=$v
			[ $i -eq 1 ] && let Y=$v
			[ $i -eq 2 ] && let Z=$v
			let i=i+1;
		done
		IFS=$IFS_SAVED
		if [ $i -eq 3 ]; then
			[ $X -gt $XM ] && let XM=$X YM=$Y ZM=$Z
			[ $X -eq $XM ] && [ $Y -gt $YM ] && let YM=$Y ZM=$Z
			[ $X -eq $XM ] && [ $Y -eq $YM ] && [ $Z -gt $ZM ] && let ZM=$Z
		fi
	done
	echo "$XM.$YM.$ZM"
}

function checkout() {
	if [ ! -e $SOURCE_FOLDER ]; then
		git clone --no-checkout $GIT_REPOSITORY $SOURCE_FOLDER || err "Failed to clone the git repository."
	fi
	
	cd $SOURCE_FOLDER
	
	if [ $GIT_VERSION == "stable" ]; then
		GIT_VERSION=$(version_max)
	elif [ $GIT_VERSION == "testing" ]; then
		GIT_VERSION="master"
	fi
	
	git checkout $GIT_VERSION || err "Failed to checkout latest version."
}

function configure-lib() {
	CWD_SAVED=$PWD
	mkdir -p $TARGET_FOLDER/libftl/$1
	cd $TARGET_FOLDER/libftl/$1
	echo
	msg "Configuring $PWD..."
	EXTRA=""
	if echo $1|grep static > /dev/null; then
		EXTRA="-D BUILD_SHARED_LIBS=OFF"
	fi
	[ $CONFIGURE_CLEAN -ne 0 ] && rm -rf CMakeCache.txt
	cmake -D CMAKE_BUILD_TYPE=Debug $EXTRA $SOURCE_FOLDER || err
	ret=$?
	cd $CWD_SAVED
	return $ret
}

function configure() {
	[ $BUILD_SHARED -ne 0 ] && configure-lib debug || err
	[ $BUILD_SHARED -ne 0 ] && configure-lib release || err
	[ $BUILD_STATIC -ne 0 ] && configure-lib debug-static || err
	[ $BUILD_STATIC -ne 0 ] && configure-lib release-static || err
}

function build-lib() {
	CWD_SAVED=$PWD
	cd $TARGET_FOLDER/libftl/$1
	echo
	msg "Building $PWD..."
	make $MAKEFLAGS ftl
	ret=$?
	cd $CWD_SAVED
	return $ret
}

function build() {
	[ $BUILD_SHARED -ne 0 ] && build-lib debug || err
	[ $BUILD_SHARED -ne 0 ] && build-lib release || err
	[ $BUILD_STATIC -ne 0 ] && build-lib debug-static || err
	[ $BUILD_STATIC -ne 0 ] && build-lib release-static || err
}

CWD_SAVED=$PWD
cd $TARGET_FOLDER
[ $CHECKOUT -ne 0 ] && checkout
[ $CONFIGURE -ne 0 ] || [ $BUILD -ne 0 ] && configure
[ $BUILD -ne 0 ] && build
cd $CWD_SAVED
