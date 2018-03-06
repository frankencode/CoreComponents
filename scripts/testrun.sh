#! /bin/bash -x

TEST_APP=exampleWindow
TEST_ARG=
TEST_ENV="DISPLAY=:0"
TEST_STATION=debby-nine
TEST_OS=${TEST_STATION}
TEST_USER=${USER}
BUILD_TYPE=debug
CC_SOURCE=~/src/cc
APP_SOURCE=${CC_SOURCE}
BUILD_DIR=~/build-${TEST_STATION}-${BUILD_TYPE}
TOOLS_DIR=~/build-${TEST_STATION}-tools
CCBUILD=${TOOLS_DIR}/ccbuild
RSYNC=/usr/bin/rsync
SSH=/usr/bin/ssh

bootstrap_ccbuild() {
    [ -d $TOOLS_DIR ] || mkdir $TOOLS_DIR
    pushd $TOOLS_DIR
    RET=0
    $CC_SOURCE/bootstrap || RET=$?
    popd
    return $RET
}

build_app() {
    [ -d $BUILD_DIR ] || mkdir $BUILD_DIR
    pushd $BUILD_DIR
    RET=0
    $CCBUILD -$BUILD_TYPE -test $APP_SOURCE || RET=$?
    popd
    return $RET
}

run_app() {
    $SSH -t ${TEST_USER}@${TEST_STATION} "while killall ${TEST_APP}; do sleep 1; done" 2>&1 > /dev/null
    $RSYNC -aHv --delete --exclude='.*' $BUILD_DIR/ ${TEST_USER}@${TEST_STATION}:${BUILD_DIR}
    $SSH -t ${TEST_USER}@${TEST_STATION} "$TEST_ENV $BUILD_DIR/$TEST_APP $TEST_ARG"
}

if [ ! -x $RSYNC ]; then
    echo "Please install rsync, first"
    exit 1
fi

if [ ! -x $SSH ]; then
    echo "Please install ssh, first (openssh-client)"
    exit 1
fi

[ -x $CCBUILD ] || bootstrap_ccbuild
build_app && run_app
