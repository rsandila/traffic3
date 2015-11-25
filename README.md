# What is Traffic?

Traffic is a network traffic generator following a server/client model
for generating high volumes of traffic on a network. This could be
used to test the ability of a router/firewall/VPN to handle continuous
high traffic loads. It is optimally used in an development
environment. It does not measure throughput or number of connections
per second or anything like this.

# Use cases

This is a network traffic generator. It is used to check what massive amounts of traffic of certain type will do to an intervening network. It does not try to measure throughput or response times. It has been made with the question in mind: If 100 clients does simultaneous TCP transfers for 2 days, will my router break? Or can I configure my firewall while 50 people are doing large TCP transfers through the device?

# Who should use it?

Responsible software developers in a test environment who needs to
know how their router/firewall/VPN holds up against continuous high
traffic loads. You should be careful to use this tool only in
environments where it will not affect the normal day to day operations
of your coworkers.

# License

GPLv2 license

# Guarantees

I have tested this software and it works for me. I can not take
responsibility for what anybody does with this software and whether
this software is fit or not for any purpose stated or not in any
documents. It is as-is. If you break it, you get to keep all the
pieces.

# Status

Currently builds on my Mac OS X, Ubuntu 14 LTS, CentOS 7 and Windows 10 systems.

Will not work on FreeBSD until premake5 is ported to FreeBSD.

## Build Status

* Travis CI Linux:
[![Travis CI Linux Build Status](https://travis-ci.org/rsandila/traffic3.svg?branch=master)](https://travis-ci.org/rsandila/traffic3.svg?branch=master)

* AppVeyor CI Windows: [![AppVeyor CI Windows Build Status](https://ci.appveyor.com/api/projects/status/uoecusx4mnetiyly?svg=true)](https://ci.appveyor.com/project/rsandila/traffic3)

*NOTE* AppVeyor seems unable to run any IPV6 related tests so those are skipped

# Requirements

1. Compiler that supports C++11
  1. gcc 4.8 or later
  1. MacOS X: clang 7 or later
  1. Visual Studio 2015
1. Linux (CentOS 7 or Ubuntu 14 LTS), Mac OS X Capitan or Windows 10
1. git
1. Build Tool:
  1. gmake (Linux and Mac OS X)
  1. Visual Studio (Windows)
  1. XCode (Mac OS X)

# How to Build

This assumes running in a terminal at the root of the source code tree.

## gmake

1. Change to 3rdparty folder
1. Run `./build.sh`

## XCode

1. Change to 3rdparty folder
1. Run `./build.sh`
1. Change to previous folder (`cd -`)
1. Run `./3rdparty/premake-core/bin/release/premake5 xcode4`
1. Run `open traffic3.xcworkspace`

## Visual Studio 2015

This assume a cmd/powershell window at the root folder of the source code. You can download [Visual Studio Community Edition](https://www.visualstudio.com/en-us/products/visual-studio-express-vs.aspx) from Microsoft. This is sufficient to compile this project.

1. Run `bin\tools\windows\premake5 vs2015`
  1. If you don't want to run the included binary, download a trusted one from the [premake5 project](https://premake.github.io/download.html#v5).
1. Run `start traffic3.sln`

# TODO

Moved to [Roadmap](https://github.com/rsandila/traffic3/wiki/Roadmap) page in the wiki.

# History

This is a new version of the Network Traffic Generator rewritten using C++11 techniques. For information on the previous version take a look at [traffic](http://robert.rsa3.com/pmwiki/pmwiki.php?n=Projects.Traffic). Currently working on getting feature parity with the older version.
