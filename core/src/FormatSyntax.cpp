/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/String>
#include <flux/Singleton>
#include <flux/FormatSyntax>

namespace flux {

FormatSyntax::FormatSyntax()
{
    SYNTAX("format");

    width_ = DEFINE("Width", REPEAT(1, 2, RANGE('0', '9')));
    integerWidth_ = DEFINE("IntegerWidth", REPEAT(1, 2, RANGE('0', '9')));
    fractionWidth_ = DEFINE("FractionWidth", REPEAT(1, 2, RANGE('0', '9')));

    base_ =
        DEFINE("Base",
            CHOICE(
                STRING("dec"),
                STRING("hex"),
                STRING("oct"),
                STRING("bin")
            )
        );

    exp_ = DEFINE("Exp", RANGE("eE"));

    blank_ = DEFINE("Blank",
        GLUE(
            CHAR('\''),
            RANGE(32, 127),
            CHAR('\'')
        )
    );

    specifier_ =
        DEFINE("Specifier",
            GLUE(
                CHAR('%'),
                REPEAT(0, 5,
                    GLUE(
                        CHOICE(
                            GLUE(
                                REF("IntegerWidth"),
                                CHAR('.'),
                                REPEAT(0, 1,
                                    REF("FractionWidth")
                                )
                            ),
                            GLUE(
                                CHAR('.'),
                                REF("FractionWidth")
                            ),
                            REF("Width"),
                            REF("Base"),
                            REF("Exp"),
                            REF("Blank")
                        ),
                        REPEAT(0, 1, CHAR(':'))
                    )
                ),
                CHAR('%')
            )
        );

    ENTRY("Specifier");
    LINK();
}

bool FormatSyntax::find(const ByteArray *text, int *i0, int *i1, int *w, int *wi, int *wf, int *base, bool *exp, char *blank) const
{
    Ref<Token> rootToken = SyntaxDefinition::find(text, *i0)->rootToken();

    if (rootToken)
    {
        if (i0) *i0 = rootToken->i0();
        if (i1) *i1 = rootToken->i1();

        Token *token = rootToken->firstChild();

        while (token)
        {
            String value = text->copy(token->i0(), token->i1());

            if (token->rule() == width_)
            {
                *w = value->toInt();
            }
            else if (token->rule() == integerWidth_)
            {
                *wi = value->toInt();
            }
            else if (token->rule() == fractionWidth_)
            {
                *wf = value->toInt();
            }
            else if (token->rule() == base_)
            {
                if (value == "dec")
                    *base = 10;
                else if (value == "hex")
                    *base = 16;
                else if (value == "oct")
                    *base = 8;
                else if (value == "bin")
                    *base = 2;
            }
            else if (token->rule() == exp_)
            {
                *exp = true;
            }
            else if (token->rule() == blank_)
            {
                *blank = value->at(1);
            }

            token = token->nextSibling();
        }
    }

    return rootToken;
}

const FormatSyntax *formatSyntax() { return Singleton<FormatSyntax>::instance(); }

} // namespace flux
