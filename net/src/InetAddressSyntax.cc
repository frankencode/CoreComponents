/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Singleton>
#include <cc/net/InetAddressSyntax>

namespace cc {
namespace net {

using namespace cc::syntax;

const InetAddressSyntax *InetAddressSyntax::instance()
{
    return Singleton<InetAddressSyntax>::instance();
}

InetAddressSyntax::InetAddressSyntax(SyntaxDebugFactory *debugFactory):
    SyntaxDefinition(debugFactory)
{
    DEFINE("DecimalOctet",
        REPEAT(0, 3, RANGE('0', '9'))
    );

    DEFINE("HexWord",
        REPEAT(1, 4,
            CHOICE(
                RANGE('0', '9'),
                RANGE('a', 'f'),
                RANGE('A', 'F')
            )
        )
    );

    inet4Address_ =
        DEFINE("Inet4Address",
            GLUE(
                INLINE("DecimalOctet"),
                CHAR('.'),
                INLINE("DecimalOctet"),
                CHAR('.'),
                INLINE("DecimalOctet"),
                CHAR('.'),
                INLINE("DecimalOctet")
            )
        );

    inet6Address_ =
        DEFINE("Inet6Address",
            GLUE(
                REPEAT(1, 7,
                    GLUE(
                        CHOICE(
                            INLINE("HexWord"),
                            CHAR(':')
                        ),
                        CHAR(':')
                    )
                ),
                CHOICE(
                    REF("Inet4Address"),
                    INLINE("HexWord")
                )
            )
        );

    inetAddress_ =
        DEFINE("InetAddress",
            CHOICE(
                REF("Inet4Address"),
                REF("Inet6Address")
            )
        );

    ENTRY("InetAddress");
    LINK();
}

}} // namespace cc::net
