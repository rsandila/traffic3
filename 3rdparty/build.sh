#!/bin/bash

git submodule init
git submodule update
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
git submodule init
git submodule update
make -f Bootstrap.mak ${OS}
popd
pushd ..
./3rdparty/premake-core/bin/release/premake5 gmake
make clean all
./test/Debug/traffic3_test
popd
