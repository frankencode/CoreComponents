CoreComponents                           {#mainpage}
==============

About
-----

CoreComponents provides a small set of libraries and tools for Unix system programming.
The entire toolkit is fully self-contained without any external dependencies other than a gcc
compatible compiler and the standard C++ runtime (C++ 2011). Development happens mostly on Linux and
OpenBSD, but design goal is to support any modern operating system which implements the
core POSIX API.

Features
--------

 * fully garbage collected
 * highlevel data countainers, without iterators
 * powerful string processing and pattern matching
 * designed for mulithreading from ground up
 * fast compile times
 * transparent API: full access to low-level system primitives
 * batteries included:
   * easy to use build system (ccbuild)
   * built-in webserver (ccnode)
     * compatible and fast (HTTP 1.1, memory mapped I/O)
     * easy to extend
   * powerful text search and replace (ccfind)

Installation
------------

Prequisites:
 * gcc or compatible compiler (e.g.: gcc, clang/llvm, suncc)
 * POSIX compatible shell (e.g.: ksh, bash)

First download the latest stable version from github:
```
mkdir -p ~/src
cd ~/src
curl -L https://www.corecomponents.io/download/cc-0.7.0.tar.gz | tar xzv
ln -s ../cc-0.7.0 cc
```

Thereafter bootstrap the build system:
```
mkdir -p ~/build/cc_debug
cd ~/build/cc_debug
~/src/cc/bootstrap
```

Finally, build the entire toolkit:
```
./ccbuild -debug ~/src/cc
```

Optionally add the build directory to your environment:
```
export PATH=$PWD:$PATH
echo "export PATH=$PWD:\$PATH" >> ~/.profile
```
