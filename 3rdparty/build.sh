#!/bin/bash

pushd ..
git submodule init || exit 9
git submodule update || exit 8
popd
OS=linux
case `uname` in
  Linux)
    OS=linux
    ;;
  Darwin)
    OS=osx
    ;;
esac
pushd premake-core
git submodule init || exit 7
git submodule update || exit 6
make -f Bootstrap.mak ${OS} || exit 5
popd
pushd ..
./3rdparty/premake-core/bin/release/premake5 gmake || exit 4
make clean all -j`getconf _NPROCESSORS_ONLN` || exit 2
./bin/32bit/Debug/traffic3_test || exit 3
popd
