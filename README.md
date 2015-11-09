# Overview

This is a new version of the Network Traffic Generator rewritten using C++11 techniques. For information on the previous version take a look at [traffic](http://robert.rsa3.com/pmwiki/pmwiki.php?n=Projects.Traffic). Currently working on getting feature parity with the older version.

# License
GPLv2 license

# Status

Serious work in progress.

Currently builds on my Mac OS X, Ubuntu 14 LTS and CentOS 7 systems.

Will not work on FreeBSD until premake5 is ported to FreeBSD.

Travis CI Status:
[![Build Status](https://travis-ci.org/rsandila/traffic3.svg?branch=master)](https://travis-ci.org/rsandila/traffic3.svg?branch=master)

# Requirements

1. Compiler that supports C++11
  1. gcc 4.8 or later
  1. MacOS X: clang 7 or later
1. Linux (CentOS 7 or Ubuntu 14 LTS) or Mac OS X Capitan
1. git
1. gmake

# How to Build

This assumes running in a terminal at the root of the source code tree.

## gmake

1. Change to 3rdparty folder
1. Run `./build.sh`

## xcode

1. Change to 3rdparty folder
1. Run `./build.sh`
1. Change to previous folder (`cd -`)
1. Run `./3rdparty/premake-core/bin/release/premake5 xcode4`
1. Run `open open traffic3.xcworkspace`

# TODO

## Version 3.0

1. ~~Add echo contentmanager~~
1. ~~Add random (binary) contentmanager~~
1. Add UDP4
1. Add TCP6
1. Add UDP6

## Version 3.1

1. Get it to work in Windows

## Version 3.2

1. Build rpm and deb files
2. Build windows installer

## Version 3.3

1. Create REST interface
