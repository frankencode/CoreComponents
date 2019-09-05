/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <math.h> // pow
#include <cc/Singleton>
#include <cc/syntax/FloatSyntax>

namespace cc {
namespace syntax {

const FloatSyntax *FloatSyntax::instance()
{
    return Singleton<FloatSyntax>::instance();
}

FloatSyntax::FloatSyntax()
{
    SYNTAX("float");

    sign_ = DEFINE("Sign", RANGE("+-"));
    integerPart_ = DEFINE("IntegerPart", REPEAT(1, 20, RANGE('0', '9')));
    fractionPart_ = DEFINE("FractionPart", REPEAT(1, 20, RANGE('0', '9')));
    exponentSign_ = DEFINE("ExponentSign", RANGE("+-"));
    exponent_ = DEFINE("Exponent",  REPEAT(1, 3, RANGE('0', '9')));
    nan_ = DEFINE("NaN", CHOICE(STRING("NaN"), STRING("nan")));
    infinite_ = DEFINE("Infinite", GLUE(REPEAT(0, 1, CHAR('-')), CHOICE(STRING("INFINITE"), STRING("inf"))));

    literal_ =
        DEFINE("Literal",
            CHOICE(
                GLUE(
                    REPEAT(0, 1, REF("Sign")),
                    CHOICE(
                        GLUE(
                            REF("IntegerPart"),
                            REPEAT(0, 1,
                                GLUE(
                                    CHAR('.'),
                                    REPEAT(0, 1, REF("FractionPart"))
                                )
                            )
                        ),
                        GLUE(
                            CHAR('.'),
                            REF("FractionPart")
                        )
                    ),
                    REPEAT(0, 1,
                        GLUE(
                            RANGE("eE"),
                            REPEAT(0, 1, REF("ExponentSign")),
                            REF("Exponent")
                        )
                    ),
                    NOT(RANGE(".eE"))
                ),
                GLUE(
                    CHOICE(
                        REF("NaN"),
                        REF("Infinite")
                    ),
                    NOT(RANGE('a', 'z')),
                    NOT(RANGE('A', 'Z'))
                )
            )
        );

    ENTRY("Literal");
    LINK();
}

void FloatSyntax::read(float64_t *value, const CharArray *text, const Token *token) const
{
    token = token->firstChild();

    if (token->rule() == nan_)
    {
        *value = cc::nan();
    }
    else if (token->rule() == infinite_)
    {
        float64_t one, zero;
        one = 1.; zero = 0.;

        if (text->at(token->i0()) == '-')
            *value = -one / zero;
        else
            *value = one / zero;
    }
    else
    {
        int sign = 1;
        float64_t mantissa = 0.;
        int epSign = 1;
        int ep = 0;

        while (token)
        {
            if (token->rule() == sign_)
            {
                if (text->at(token->i0()) == '-')
                    sign = -1;
            }
            else if (token->rule() == integerPart_)
            {
                for (int i = token->i0(); i < token->i1(); ++i)
                {
                    mantissa *= 10;
                    mantissa += text->at(i) - '0';
                }
            }
            else if (token->rule() == fractionPart_)
            {
                float64_t h = 0.1;
                for (int i = token->i0(); i < token->i1(); ++i, h /= 10)
                    mantissa += h * (text->at(i) - '0');
            }
            else if (token->rule() == exponentSign_)
            {
                if (text->at(token->i0()) == '-')
                    epSign = -1;
            }
            else if (token->rule() == exponent_)
            {
                for (int i = token->i0(); i < token->i1(); ++i)
                {
                    ep *= 10;
                    ep += text->at(i) - '0';
                }
            }

            token = token->nextSibling();
        }

        *value = sign * mantissa;
        if (ep != 0)
            *value *= ::pow(10., float64_t(epSign * ep));
    }
}

Ref<Token> FloatSyntax::read(float64_t *value, const CharArray *text, int i) const
{
    Ref<Token> token = match(text, i)->rootToken();
    if (token) read(value, text, token);
    return token;
}

}} // namespace cc::syntax
