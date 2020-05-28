/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "CHeaderStyle.h"
#include <cc/Singleton>
#include <cc/File>
#include <cc/Format>
#include <cc/syntax/SyntaxDefinition>

namespace ccclaim {

class CHeaderSyntax: public SyntaxDefinition
{
public:
    CHeaderSyntax()
    {
        DEFINE("Comment",
            GLUE(
                STRING("/*"),
                FIND(STRING("*/"))
            )
        );

        DEFINE("Header",
            GLUE(
                REPEAT(RANGE(" \t\n\r")),
                REF("Comment"),
                REPEAT(RANGE(" \t\n\r"))
            )
        );

        ENTRY("Header");

        LINK();
    }
};

CHeaderStyle::CHeaderStyle():
    HeaderStyle{"cxx"}
{}

Ref<Header> CHeaderStyle::scan(const String &path) const
{
    String text = File::open(path)->map();
    Ref<Token> rootToken = Singleton<CHeaderSyntax>::instance()->match(text, 0)->rootToken();
    if (!rootToken) return Ref<Header>();
    Token *token = rootToken->firstChild();
    String message = trimHeader(text->copy(token->i0() + 2, token->i1() - 2), " \t\r*");
    return Header::create(path, rootToken, text, message);
}

String CHeaderStyle::str(const Notice *notice) const
{
    Format f;
    f << "/*\n";
    CopyrightList *copyrights = notice->copyrights();
    for (int i = 0; i < copyrights->count(); ++i) {
        Copyright *c = copyrights->at(i);
        f << " * Copyright (C) ";
        if (c->yearStart() == c->yearEnd()) f << c->yearStart();
        else f << c->yearStart() << "-" << c->yearEnd();
        f << " " << c->holder() << ".";
        if (i != copyrights->count() - 1) f << "\n";
    }
    /*if (copyrights->count() == 1 && !notice->statement()->contains('\n')) {
        f << "  " << notice->statement() << ".\n";
    }
    else {*/
        f << "\n"
            " *\n"
            " * " << notice->statement()->replace("\n", "\n * ") << "\n";
    //}
    f <<
        " *\n"
        " */\n"
        "\n";
    return f;
}

} // namespace ccclaim
