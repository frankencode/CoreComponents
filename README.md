*fluxkit:* Frank's lightweight POSIX components
===============================================

A C++ Developers Swiss-Army Knife
---------------------------------

**_fluxkit_** provides a small set of libraries and tools for Unix system programming.
The entire toolkit is fully self-contained without any external depedencies other than a gcc
compatible compiler and a standard C++ runtime.

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
     * inspired by nodejs
     * compatible and fast (HTTP 1.1, memory mapped I/O)
   * powerful text search and replace (fluxfind)

Installation
------------

Prequisites:
 * gcc or compatible compiler (e.g.: gcc, clang/llvm, suncc)
 * POSIX compatible shell (e.g.: ksh, bash)

First download the latest stable version from github:
```bash
mkdir -p ~/src
cd ~/src
curl -L https://github.com/frankencode/fluxkit/archive/v0.2.0.tar.gz | tar xzv
ln -s ../fluxkit-0.2.0 fluxkit
```

Thereafter bootstrap the build system:
```bash
mkdir -p ~/build/fluxkit_debug
cd ~/build/fluxkit_debug
~/src/fluxkit-0.2.0/bootstrap
```

Finally, build the entire toolkit:
```bash
./fluxmake -debug ~/src/fluxkit
```

Optionally add the build directory to your environment:
```bash
export PATH=$PWD:$PATH
echo "export PATH=$PWD:\$PATH" >> ~/.profile
```
