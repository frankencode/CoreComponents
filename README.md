CoreComponents
==============

About
-----

CoreComponents provides a small set of libraries and tools for Unix system programming.

Features
--------

 * fully incrementally garbage collected
 * highlevel/safe-to-use data countainers
 * powerful string processing and pattern matching
 * designed for multi-threading from ground up
 * fast compile times
 * transparent API: full access to low-level system primitives
 * batteries included:
   * easy to use build system (cc_build)
   * built-in web server (cc_http)
   * many more useful tools

Installation
------------

Prequisites:
 * gcc or compatible compiler (e.g.: gcc, clang/llvm, suncc)
 * POSIX compatible shell (e.g.: ksh, bash)
 * pkg-config and gnureadline (optional)

Install required development packages:

```
sudo apt-get install g++ pkg-config git curl
```

Download the latest stable version from github:
```
mkdir -p ~/src
cd ~/src
curl -L https://www.corecomponents.io/download/cc-2.0.0.tar.gz | tar xzv
ln -s ../cc-2.0.0 cc
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
