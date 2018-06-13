CoreComponents
==============

About
-----

CoreComponents provides a small set of libraries and tools for Unix system programming.
The entire toolkit is fully self-contained without external dependencies other than a gcc
compatible compiler and the standard C++ runtime (C++11). Development happens mostly on Linux and
OpenBSD, but design goal is to support any modern operating system which implements the
core POSIX API.

Features
--------

 * fully incrementally garbage collected
 * highlevel/safe-to-use data countainers
 * powerful string processing and pattern matching
 * designed for multi-threading from ground up
 * fast compile times
 * transparent API: full access to low-level system primitives
 * batteries included:
   * easy to use build system (ccbuild)
   * built-in web server (ccnode)
   * powerful text search and replace (ccfind)

Installation
------------

Prequisites:
 * gcc or compatible compiler (e.g.: gcc, clang/llvm, suncc)
 * POSIX compatible shell (e.g.: ksh, bash)
 * pkg-config and gnureadline (optional)

Install required development packages:

```
sudo apt-get install g++ pkg-config libreadline-dev git curl
```

Download the latest stable version from github:
```
mkdir -p ~/src
cd ~/src
curl -L https://www.corecomponents.io/download/cc-0.14.0.tar.gz | tar xzv
ln -s ../cc-0.14.0 cc
```

Thereafter bootstrap the build system:
```
mkdir -p ~/build/cc_release
cd ~/build/cc_release
~/src/cc/bootstrap
```

Finally, build the entire toolkit:
```
./ccbuild -release -test ~/src/cc
```

Optionally add the build directory to your environment:
```
export PATH=$PWD:$PATH
echo "export PATH=$PWD:\$PATH" >> ~/.profile
```
