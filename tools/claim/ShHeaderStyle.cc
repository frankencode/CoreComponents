/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Singleton>
#include <cc/File>
#include <cc/Format>
#include <cc/syntax/SyntaxDefinition>
#include "ShHeaderStyle.h"

namespace ccclaim {

class ShHeaderSyntax: public SyntaxDefinition
{
public:
    ShHeaderSyntax()
    {
        SYNTAX("sh-header");

        DEFINE("Magic",
            GLUE(
                REPEAT(RANGE(" \t")),
                STRING("#!"),
                FIND(
                    CHAR('\n')
                )
            )
        );

        DEFINE("Header",
            GLUE(
                REPEAT(1,
                    GLUE(
                        REPEAT(RANGE(" \t")),
                        CHAR('#'),
                        FIND(
                            CHAR('\n')
                        )
                    )
                ),
                REPEAT(RANGE(" \t\n\r"))
            )
        );

        DEFINE("Script",
            GLUE(
                REPEAT(0, 1, INLINE("Magic")),
                REF("Header")
            )
        );

        ENTRY("Script");

        LINK();
    }
};

class ShMagicSyntax: public SyntaxDefinition
{
public:
    ShMagicSyntax()
    {
        SYNTAX("sh-magic");
        IMPORT(Singleton<ShHeaderSyntax>::instance());
        DEFINE("Magic", INLINE("sh-header::Magic"));
        ENTRY("Magic");
        LINK();
    }
};

ShHeaderStyle::ShHeaderStyle():
    HeaderStyle{"sh"}
{}

Ref<Header> ShHeaderStyle::scan(const String &path) const
{
    String text = File{path}->map();
    Ref<Token> rootToken = Singleton<ShHeaderSyntax>::instance()->match(text, 0)->rootToken();
    if (!rootToken) return Ref<Header>();
    Token *token = rootToken->firstChild();
    String message = trimHeader(text->copy(token->i0(), token->i1()), " \t\r#");
    return Header::create(path, token, text, message);
}

String ShHeaderStyle::str(const Notice *notice) const
{
    Format f;
    f << "##\n";
    CopyrightList *copyrights = notice->copyrights();
    for (int i = 0; i < copyrights->count(); ++i) {
        Copyright *c = copyrights->at(i);
        f << " # Copyright (C) ";
        if (c->yearStart() == c->yearEnd()) f << c->yearStart();
        else f << c->yearStart() << "-" << c->yearEnd();
        f << " " << c->holder() << ".\n";
    }
    f <<
        " #\n"
        " # " << notice->statement()->replace("\n", "\n # ") << "\n";
    f <<
        " #\n"
        " ##\n"
        "\n";
    return f;
}

int ShHeaderStyle::magicCount(const String &text) const
{
    Ref<Token> token = Singleton<ShMagicSyntax>::instance()->match(text, 0)->rootToken();
    if (!token) return 0;
    return token->i1();
}

} // namespace ccclaim
