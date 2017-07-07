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

class ShSyntax: public SyntaxDefinition
{
protected:
    friend class Singleton<ShSyntax>;
    ShSyntax();
};

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

const SyntaxDefinition *shSyntax() { return Singleton<ShSyntax>::instance(); }

}} // namespace cc::toki
