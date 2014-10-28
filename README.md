fluxkit: Frank's lightweight POSIX components
==============================================

A C++ Developers Swiss-Army Knife
---------------------------------

**fluxkit** provides a small set of libraries and tools for Unix system programming.
The entire toolkit is fully self-contained without any external depedencies other than gcc and
a standard C++ runtime.

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
     * compatible and fast (memory mapped I/O and HTTP 1.1)
   * powerful text search and replace (fluxfind)

Installation
------------

Prequisites:
 * gcc or compatible compiler
 * POSIX compatible shell (e.g.: ksh, bash)

First download the latest stable version from github:
```
mkdir -p ~/src
cd ~/src
curl -L https://github.com/frankencode/fluxkit/archive/v0.2.0.tar.gz | tar xzv
ln -s ../fluxkit-0.2.0 fluxkit
```

Thereafter bootstrap the build system:
```
mkdir -p ~/build/fluxkit
cd ~/build/fluxkit
~/src/fluxkit-0.2.0/bootstrap
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
