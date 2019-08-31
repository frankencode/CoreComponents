/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Singleton>
#include <cc/toki/ShSyntax>

namespace cc {
namespace toki {

const ShSyntax *ShSyntax::instance()
{
    return Singleton<ShSyntax>::instance();
}

ShSyntax::ShSyntax()
{
    SYNTAX("sh");

    DEFINE("Comment",
        GLUE(
            CHAR('#'),
            FIND(
                CHOICE(
                    CHAR('\n'),
                    EOI()
                )
            )
        )
    );

    DEFINE("Source",
        REPEAT(
            CHOICE(
                REF("Comment"),
                ANY()
            )
        )
    );

    ENTRY("Source");
    LINK();
}

}} // namespace cc::toki
