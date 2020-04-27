/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Singleton>
#include <cc/net/InetAddressSyntax>
#include <cc/net/UriSyntax>

namespace cc {
namespace net {

using namespace cc::syntax;

const UriSyntax *UriSyntax::instance()
{
    return Singleton<UriSyntax>::instance();
}

UriSyntax::UriSyntax(SyntaxDebugFactory *debugFactory):
    SyntaxDefinition{debugFactory}
{
    SYNTAX("uri");
    IMPORT(InetAddressSyntax::instance(), "inet");

    DEFINE("ALPHA",
        CHOICE(
            RANGE('a', 'z'), // 0x61 - 0x7A
            RANGE('A', 'Z')  // 0x41 - 0x5A
        )
    );

    DEFINE("DIGIT",
        RANGE('0', '9') // 0x30 - 0x39
    );

    DEFINE("HEXDIG",
        CHOICE(
            INLINE("DIGIT"),
            CHOICE(
                RANGE('a', 'f'), // 0x61 - 0x66
                RANGE('A', 'F')  // 0x41 - 0x46
            )
        )
    );

    DEFINE("Delimiter",
        RANGE(":/?#[]@")
    );

    DEFINE("Punctuator",
        RANGE("!$&'()*+,;=")
    );

    /*DEFINE("Reserved",
        CHOICE(
            INLINE("Delimiter"),
            INLINE("Punctuator")
        )
    );*/

    DEFINE("Unreserved",
        CHOICE(
            INLINE("ALPHA"),
            INLINE("DIGIT"),
            RANGE("-._~")
        )
    );

    DEFINE("PathChar",
        CHOICE(
            INLINE("Unreserved"),
            INLINE("PercentEncoded"),
            INLINE("Punctuator"),
            RANGE(":@")
        )
    );

    DEFINE("PercentEncoded",
        GLUE(
            CHAR('%'),
            INLINE("HEXDIG"),
            INLINE("HEXDIG")
        )
    );

    userInfo_ =
        DEFINE("UserInfo",
            REPEAT(
                CHOICE(
                    INLINE("Unreserved"),
                    INLINE("PercentEncoded"),
                    INLINE("Punctuator"),
                    CHAR(':')
                )
            )
        );

    host_ =
        DEFINE("Host",
            CHOICE(
                REF("inet::Inet6Address"),
                REF("inet::Inet4Address"),
                REPEAT(1,
                    CHOICE(
                        INLINE("Unreserved"),
                        INLINE("PercentEncoded"),
                        INLINE("Punctuator")
                    )
                )
            )
        );

    port_ =
        DEFINE("Port",
            REPEAT(1,
                INLINE("DIGIT")
            )
        );

    scheme_ =
        DEFINE("Scheme",
            GLUE(
                INLINE("ALPHA"),
                REPEAT(
                    CHOICE(
                        INLINE("ALPHA"),
                        INLINE("DIGIT"),
                        RANGE("+-.")
                    )
                )
            )
        );

    authority_ =
        DEFINE("Authority",
            GLUE(
                REPEAT(0, 1,
                    GLUE(
                        REF("UserInfo"),
                        CHAR('@')
                    )
                ),
                CHOICE(
                    GLUE(
                        CHAR('['),
                        REF("Host"),
                        CHAR(']')
                    ),
                    REF("Host")
                ),
                REPEAT(0, 1,
                    GLUE(
                        CHAR(':'),
                        REF("Port")
                    )
                )
            )
        );

    path_ =
        DEFINE("Path",
            GLUE(
                REPEAT(
                    INLINE("PathChar")
                ),
                REPEAT(
                    GLUE(
                        CHAR('/'),
                        REPEAT(
                            INLINE("PathChar")
                        )
                    )
                )
            )
        );

    query_ =
        DEFINE("Query",
            REPEAT(
                CHOICE(
                    INLINE("PathChar"),
                    RANGE("/?")
                )
            )
        );

    fragment_ =
        DEFINE("Fragment",
            REPEAT(
                CHOICE(
                    INLINE("PathChar"),
                    RANGE("/?")
                )
            )
        );

    uri_ =
        DEFINE("Uri",
            GLUE(
                REPEAT(0, 1,
                    GLUE(
                        REF("Scheme"),
                        CHAR(':'),
                        AHEAD(STRING("//"))
                    )
                ),
                REPEAT(0, 1,
                    GLUE(
                        REPEAT(0, 1, STRING("//")),
                        REF("Authority")
                    )
                ),
                REF("Path"),
                REPEAT(0, 1,
                    GLUE(
                        CHAR('?'),
                        REF("Query")
                    )
                ),
                REPEAT(0, 1,
                    GLUE(
                        CHAR('#'),
                        REF("Fragment")
                    )
                )
            )
        );

    ENTRY("Uri");
    LINK();
}

}} // namespace cc::net
