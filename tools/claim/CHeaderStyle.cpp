/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/Singleton>
#include <flux/File>
#include <flux/Format>
#include <flux/syntax/SyntaxDefinition>
#include "CHeaderStyle.h"

namespace fluxclaim {

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
    HeaderStyle("cxx")
{}

Ref<Header> CHeaderStyle::scan(String path) const
{
    String text = File::open(path)->map();
    Ref<Token> rootToken = Singleton<CHeaderSyntax>::instance()->match(text, 0)->rootToken();
    if (!rootToken) return Ref<Header>();
    Token *token = rootToken->firstChild();
    String message = trimHeader(text->copy(token->i0() + 2, token->i1() - 2), " \t\r*");
    return Header::create(path, rootToken, text, message);
}

String CHeaderStyle::str(Notice *notice) const
{
    Format format;
    format << "/*\n";
    CopyrightList *copyrights = notice->copyrights();
    for (int i = 0; i < copyrights->count(); ++i) {
        Copyright *c = copyrights->at(i);
        format << " * Copyright (C) ";
        if (c->yearStart() == c->yearEnd()) format << c->yearStart();
        else format << c->yearStart() << "-" << c->yearEnd();
        format << " " << c->holder() << ".";
        if (i != copyrights->count() - 1) format << "\n";
    }
    /*if (copyrights->count() == 1 && !notice->statement()->contains('\n')) {
        format << "  " << notice->statement() << ".\n";
    }
    else {*/
        format << "\n"
            " *\n"
            " * " << notice->statement()->replace("\n", "\n * ") << "\n";
    //}
    format <<
        " *\n"
        " */\n"
        "\n";
    return format;
}

} // namespace fluxclaim
