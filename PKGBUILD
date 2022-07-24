# Maintainer: Frank Mertens <frank at cyblogic dot de>

pkgbase=corecomponents
pkgname=(
    'corecomponents_core'
    'corecomponents_core_tools'
    'corecomponents_syntax'
    'corecomponents_glob'
    'corecomponents_meta'
    'corecomponents_testing'
    'corecomponents_crypto'
    'corecomponents_crypto_tools'
    'corecomponents_network'
    'corecomponents_network_tools'
    'corecomponents_http'
    'corecomponents_http_plugins'
    'corecomponents_http_tools'
    'corecomponents_toki'
    'corecomponents_toki_tools'
    'corecomponents_ux'
    'corecomponents_ux_plugins'
    'corecomponents_tools'
)
pkgver=2.2.0
pkgrel=1
pkgdesc="Toolkit for C++ application development"
url="https://www.corecomponents.io"
arch=('x86_64')
license=('GPL3')
source=(
    "https://www.corecomponents.io/download/CoreComponents-$pkgver.tar.gz"
)
md5sums=(
    '73204baab8c21435950457e333d25749'
)
sha1sums=(
    'd8c79f1d8936f8eccc4a536ef9f7fcd91e2e0c04'
)

makedepends=(
    'gcc>=10.0.0'
    'readline'
    'gnutls>=3.3.5'
    'cairo'
    'freetype2'
    'libwebp'
    'sdl2>=2.0.10'
)

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
    ../bootstrap/ccbuild -prefix=/usr -release -test ../CoreComponents/tools
    ./ccbuild -prefix=/usr -release -test ../CoreComponents
}

package_corecomponents_core() {
    pkgdesc="$pkgdesc: system abstraction and language primitives"
    cd core_release
    ../bootstrap/ccbuild -root=$pkgdir -install -release -test ../CoreComponents/Core/src
}

package_corecomponents_core_tools() {
    pkgdesc="$pkgdesc: misc. command line utilities"
    depends=('corecomponents_core')
    cd core_release
    ../bootstrap/ccbuild -root=$pkgdir -install -release -test ../CoreComponents/Core/tools
}

package_corecomponents_syntax() {
    pkgdesc="$pkgdesc: building blocks for building recursive descent parsers"
    depends=('corecomponents_core')
    cd core_release
    ./ccbuild -root=$pkgdir -install -release -test ../CoreComponents/Syntax/src
}

package_corecomponents_glob() {
    pkgdesc="$pkgdesc: string and file name pattern matching"
    depends=('corecomponents_syntax')
    cd core_release
    ./ccbuild -root=$pkgdir -install -release -test ../CoreComponents/Glob/src
}

package_corecomponents_meta() {
    pkgdesc="$pkgdesc: object serialization, deserialization and validation"
    depends=('corecomponents_syntax')
    cd core_release
    ./ccbuild -root=$pkgdir -install -release -test ../CoreComponents/Meta/src
}

package_corecomponents_testing() {
    pkgdesc="$pkgdesc: unit testing and test automation"
    depends=('corecomponents_testing')
    cd core_release
    ./ccbuild -root=$pkgdir -install -release -test ../CoreComponents/Testing/src
}

package_corecomponents_crypto() {
    pkgdesc="$pkgdesc: cryptographic primitives"
    depends=('corecomponents_core')
    cd core_release
    ./ccbuild -root=$pkgdir -install -release -test ../CoreComponents/Crypto/src
}

package_corecomponents_crypto_tools() {
    pkgdesc="$pkgdesc: cryptographic tools"
    depends=('corecomponents_crypto' 'readline')
    cd core_release
    ./ccbuild -root=$pkgdir -install -release -test ../CoreComponents/Crypto/tools
}

package_corecomponents_network() {
    pkgdesc="$pkgdesc: network sockets, interface status and routing information"
    depends=('corecomponents_glob')
    cd core_release
    ./ccbuild -root=$pkgdir -install -release -test ../CoreComponents/Network/src
}

package_corecomponents_network_tools() {
    pkgdesc="$pkgdesc: networking tools"
    depends=('corecomponents_network')
    cd core_release
    ./ccbuild -root=$pkgdir -install -release -test ../CoreComponents/Network/tools
}

package_corecomponents_http() {
    pkgdesc="$pkgdesc: http(s) client/server classes"
    depends=('corecomponents_meta' 'corecomponents_glob' 'corecomponents_network' 'gnutls>=3.3.5')
    cd core_release
    ./ccbuild -root=$pkgdir -install -release -test ../CoreComponents/HTTP/src
}

package_corecomponents_http_plugins() {
    pkgdesc="$pkgdesc: http(s) server plugins"
    depends=('corecomponents_http')
    cd core_release
    ./ccbuild -root=$pkgdir -install -release -test ../CoreComponents/HTTP/plugins
}

package_corecomponents_http_tools() {
    pkgdesc="$pkgdesc: http(s) tools"
    depends=('corecomponents_http_plugins')
    cd core_release
    ./ccbuild -root=$pkgdir -install -release -test ../CoreComponents/HTTP/tools
}

package_corecomponents_toki() {
    pkgdesc="$pkgdesc: syntax highlighting library"
    depends=('corecomponents_meta' 'corecomponents_glob')
    cd core_release
    ./ccbuild -root=$pkgdir -install -release -test ../CoreComponents/Toki/src
}

package_corecomponents_toki_tools() {
    pkgdesc="$pkgdesc: syntax highlighting tools"
    depends=('corecomponents_toki')
    cd core_release
    ./ccbuild -root=$pkgdir -install -release -test ../CoreComponents/Toki/tools
}

package_corecomponents_ux() {
    pkgdesc="$pkgdesc: graphical user interfaces"
    depends=('corecomponents_core' 'cairo' 'freetype2' 'libwebp')
    cd core_release
    ./ccbuild -root=$pkgdir -install -release -test ../CoreComponents/UX/src
}

package_corecomponents_ux_plugins() {
    pkgdesc="$pkgdesc: graphical user interface style and platform plugins"
    depends=('corecomponents_ux' 'sdl2>=2.0.10')
    cd core_release
    ./ccbuild -root=$pkgdir -install -release -test ../CoreComponents/UX/plugins
}

package_corecomponents_tools() {
    pkgdesc="$pkgdesc: common developer tools (build system, etc.)"
    depends=('corecomponents_tools')
    cd core_release
    ./ccbuild -root=$pkgdir -install -release -test ../CoreComponents/tools
}
