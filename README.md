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

Installation from Source
------------------------

Minimal prerequisites:
 * gcc or compatible compiler (e.g.: gcc, clang/llvm)
 * POSIX compatible shell (e.g.: ksh, bash)
 * pkg-config and gnureadline (optional)

Install required development packages, e.g. on Debian/Ubuntu:

```
sudo apt-get install g++ pkg-config git curl
```

Download the latest stable version:
```
mkdir -p ~/src
cd ~/src
curl -L https://www.corecomponents.io/download/CoreComponents-3.0.0.tar.gz | tar xzv
ln -s ../CoreComponents-3.0.0 CoreComponents
```

Thereafter bootstrap the build system (ccbuild):
```
mkdir -p ~/build/CoreComponents_release
cd ~/build/CoreComponents_release
~/src/CoreComponents/bootstrap
```

Finally, build the entire toolkit:
```
./ccbuild -release -test ~/src/CoreComponents
```

The build system will only build the parts of CoreComponents which configure completely.
At this point you should have at least a minimal build of CoreComponents.
See below to learn how to install the complete list of prerequisites and build all
sub-libraries.

When you are done you might optionally add the build directory to your environment:
```
export PATH=$PWD:$PATH
echo "export PATH=$PWD:\$PATH" >> ~/.profile
```

Full Installation
-----------------

Enter the build directory and run the configure stage of the build:
```
cd ~/build/CoreComponents_release
./ccbuild -configure -release -test ~/src/CoreComponents
```

Your should get an output like the following:
~~~~~~~~~
/home/fme/src/CoreComponents/Core/src/Recipe:
  compile-flags: -fmodules-ts

  compile-flags: -std=gnu++23 -Wno-psabi

  compile-flags: -D_GNU_SOURCE

/home/fme/src/CoreComponents/HTTP/src/Recipe:
  gnutls.compile-flags: -I/usr/include/p11-kit-1
  gnutls.link-flags: -lgnutls
  gnutls.version: 3.8.5

/home/fme/src/CoreComponents/Crypto/tools/aes/Recipe:
  readline.compile-flags: -D_DEFAULT_SOURCE -D_XOPEN_SOURCE=600
  readline.link-flags: -lreadline
  readline.version: 8.2.0

/home/fme/src/CoreComponents/Zip/src/Recipe:
  libzip.link-flags: -lzip
  libzip.version: 1.10.1

/home/fme/src/CoreComponents/UX/src/Recipe:
  cairo.compile-flags: -I/usr/include/cairo -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/harfbuzz -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/sysprof-6 -pthread -I/usr/include/pixman-1
  cairo.link-flags: -lcairo
  cairo.version: 1.18.0

  freetype2.compile-flags: -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/harfbuzz -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/sysprof-6 -pthread
  freetype2.link-flags: -lfreetype
  freetype2.version: 26.1.20

  libwebp.compile-flags: -I/usr/include/webp
  libwebp.link-flags: -lwebp
  libwebp.version: 1.4.0

/home/fme/src/CoreComponents/UX/plugins/Platform/SDL/src/Recipe:
  sdl2.compile-flags: -I/usr/include/SDL2 -D_REENTRANT
  sdl2.link-flags: -lSDL2
  sdl2.version: 2.30.3

/home/fme/src/CoreComponents/UX/plugins/Image/Rsvg/src/Recipe:
  librsvg-2.0.compile-flags: -I/usr/include/librsvg-2.0 -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/cairo -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/libmount -I/usr/include/blkid -I/usr/include/sysprof-6 -pthread -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/harfbuzz -I/usr/include/pixman-1
  librsvg-2.0.link-flags: -lrsvg-2 -lm -lgio-2.0 -lglib-2.0 -lgdk_pixbuf-2.0 -lgobject-2.0 -lcairo
  librsvg-2.0.version: 2.58.0

/home/fme/src/CoreComponents/Pulse/src/Recipe:
  libpulse.compile-flags: -D_REENTRANT
  libpulse.link-flags: -lpulse -pthread
  libpulse.version: 17.0.0
~~~~~~~~~

If you see warning messages about missing packages you can find those packages by searching for the pkg-config files (*.pc). On Ubuntu/Debian you can install the `apt-file` utility to search for files in the package
database:
```
sudo apt-get update
sudo apt-get install apt-file
sudo apt-file update
```
E.g. if the `cairo` developer package would be missing you could find the corresponding package name with:
```
apt-file search cairo.pc
```

After installation of the missing packages you can restart the build process with:
```
cd ~/build/CoreComponents_release
./ccbuild -configure -release -test -insight ~/src/CoreComponents
```

The `-insight` option is optional. It will make `ccbuild` generate `compile_commands.json` databases for clang based code insight. This works with any modern editor, e.g. kate, QtCreator or Visual Studio Code (clangd plugin).
