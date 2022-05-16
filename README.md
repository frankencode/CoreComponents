CoreComponents
==============

About
-----

CoreComponents provides a common system abstraction layer on top of POSIX for C++ developers.

Features
--------

 * high-level / safe-to-use data containers
 * powerful string processing and pattern matching
 * safe memory management idioms
 * designed for multi-threading from ground up
 * fast compile times
 * transparent API: full access to low-level system primitives
 * batteries included:
   * easy to use build system (ccbuild)
   * built-in web server and client (ccget, ccnode)
   * many more useful tools

Installation
------------

Prerequisites:
 * gcc or compatible compiler (e.g.: gcc, clang/llvm)
 * POSIX compatible shell (e.g.: ksh, bash)
 * pkg-config and gnureadline (optional)

Install required development packages:

```
sudo apt-get install g++ pkg-config git curl
```

Download the latest stable version:
```
mkdir -p ~/src
cd ~/src
curl -L https://www.corecomponents.io/download/CoreComponents-2.0.6.tar.gz | tar xzv
ln -s ../CoreComponents-2.0.6 CoreComponents
```

Thereafter bootstrap the build system:
```
mkdir -p ~/build/CoreComponents_release
cd ~/build/CoreComponents_release
~/src/CoreComponents/bootstrap
```

Finally, build the entire toolkit:
```
./ccbuild -release -test ~/src/CoreComponents
```

Optionally add the build directory to your environment:
```
export PATH=$PWD:$PATH
echo "export PATH=$PWD:\$PATH" >> ~/.profile
```
