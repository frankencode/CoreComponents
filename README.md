*fluxkit:* Frank's lightweight portable components
==================================================

A C++ Developer's Swiss-Army Knife
----------------------------------

**_fluxkit_** provides a small set of libraries and tools for Unix system programming.
The entire toolkit is fully self-contained without any external depedencies other than a gcc
compatible compiler and a standard C++ runtime. Development happens mostly on Linux and
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
   * easy to use build system (fluxmake)
   * built-in webserver (fluxnode)
     * compatible and fast (HTTP 1.1, memory mapped I/O)
     * easy to extend
   * powerful text search and replace (fluxfind)

Installation
------------

Prequisites:
 * gcc or compatible compiler (e.g.: gcc, clang/llvm, suncc)
 * POSIX compatible shell (e.g.: ksh, bash)

First download the latest stable version from github:
```
mkdir -p ~/src
cd ~/src
curl -L https://github.com/frankencode/fluxkit/archive/v0.2.4.tar.gz | tar xzv
ln -s ../fluxkit-0.2.4 fluxkit
```

Thereafter bootstrap the build system:
```
mkdir -p ~/build/fluxkit_debug
cd ~/build/fluxkit_debug
~/src/fluxkit/bootstrap
```

Finally, build the entire toolkit:
```
./fluxmake -debug ~/src/fluxkit
```

Optionally add the build directory to your environment:
```
export PATH=$PWD:$PATH
echo "export PATH=$PWD:\$PATH" >> ~/.profile
```
