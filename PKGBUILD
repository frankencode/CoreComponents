# Maintainer: Frank Mertens <frank@cyblogic.de>

pkgbase=corecomponents
pkgname=(
    'corecomponents_core'
    'corecomponents_core_tools'
    'corecomponents_syntax'
    'corecomponents_glob'
    'corecomponents_meta'
    'corecomponents_crypto'
    'corecomponents_crypto_tools'
)
pkgver=2.1.1
pkgrel=1
pkgdesc="Toolkit for C++ application development"
url="https://www.corecomponents.io"
arch=('x86_64')
license=('GPL3')
source=(
    "https://www.corecomponents.io/download/CoreComponents-$pkgver.tar.gz"
    "https://www.corecomponents.io/download/CoreComponents-$pkgver.hash"
)
sha1sums=(
    '1db9ee8ca661567e35d07c4f51abe269242d56e1'
    'SKIP'
)

depends_corecomponents_cypto_tools=('readline')

prepare() {
    rm -rf CoreComponents
    ln -s CoreComponents-$pkgver CoreComponents
    rm -rf bootstrap
    mkdir bootstrap
    cd bootstrap
    ../CoreComponents-$pkgver/bootstrap
}

build() {
    rm -rf core_release
    mkdir core_release
    cd core_release
    ../bootstrap/ccbuild -prefix=/usr -release -test -insight ../CoreComponents
}

package_corecomponents_core() {
    cd core_release
    ../bootstrap/ccbuild -root=$pkgdir -install -release -test -insight ../CoreComponents/Core/src
}

package_corecomponents_core_tools() {
    cd core_release
    ../bootstrap/ccbuild -root=$pkgdir -install -release -test -insight ../CoreComponents/Core/tools
}

package_corecomponents_syntax() {
    cd core_release
    ../bootstrap/ccbuild -root=$pkgdir -install -release -test -insight ../CoreComponents/Syntax/src
}

package_corecomponents_glob() {
    cd core_release
    ../bootstrap/ccbuild -root=$pkgdir -install -release -test -insight ../CoreComponents/Glob/src
}

package_corecomponents_meta() {
    cd core_release
    ../bootstrap/ccbuild -root=$pkgdir -install -release -test -insight ../CoreComponents/Meta/src
}

package_corecomponents_crypto() {
    cd core_release
    ../bootstrap/ccbuild -root=$pkgdir -install -release -test -insight ../CoreComponents/Meta/src
}

package_corecomponents_crypto_tools() {
    cd core_release
    ../bootstrap/ccbuild -root=$pkgdir -install -release -test -insight ../CoreComponents/Meta/src
}
