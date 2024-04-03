/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/FloatSyntax>
#include <limits>
#include <cmath>

namespace cc {

struct FloatSyntax::State: public SyntaxDefinition::State
{
    SyntaxRule plusMinus {
        OneOf{'+', '-'}
    };

    SyntaxRule integralPart {
        Repeat{1, 20,
            Within{'0', '9'}
        }
    };

    SyntaxRule fractionalPart {
        Repeat{1, 20,
            Within{'0', '9'}
        }
    };

    SyntaxRule exponentPlusMinus {
        OneOf{'+', '-'}
    };

    SyntaxRule exponent {
        Repeat{1, 3,
            Within{'0', '9'}
        }
    };

    SyntaxRule notANumber {
        Casefree{"nan"}
    };

    SyntaxRule infinite {
        Sequence{
            Repeat{0, 1, Char{'-'}},
            Casefree{"inf"},
            Repeat{0, 1, Casefree{"inite"}}
        }
    };

    SyntaxRule floatingPoint {
        Choice{
            Sequence{
                Repeat{0, 1, &plusMinus},
                Choice{
                    Sequence{
                        &integralPart,
                        Repeat{0, 1,
                            Sequence{
                                Char{'.'},
                                Repeat{0, 1, &fractionalPart}
                            }
                        }
                    },
                    Sequence{
                        Char{'.'},
                        &fractionalPart
                    }
                },
                Repeat{0, 1,
                    Sequence{
                        OneOf{'e', 'E'},
                        Repeat{0, 1, &exponentPlusMinus},
                        &exponent
                    }
                },
                Not{
                    OneOf{'.', 'e', 'E'}
                }
            },
            Sequence{
                Choice{
                    &notANumber,
                    &infinite
                },
                Not{
                    Choice{
                        Within{'a', 'z'},
                        Within{'A', 'Z'}
                    }
                }
            }
        }
    };

    State():
        SyntaxDefinition::State{&floatingPoint}
    {}

    void read(Out<double> value, const String &text, const Token &token) const
    {
        auto it = token.children().begin();
        Token child = *it;

        if (child.rule() == notANumber)
        {
            value = std::numeric_limits<double>::quiet_NaN();
        }
        else if (child.rule() == infinite)
        {
            double one, zero;
            one = 1.; zero = 0.;

            if (text.at(child) == '-')
                value = -one / zero;
            else
                value = one / zero;
        }
        else {
            int sign = 1;
            double mantissa = 0.;
            int epSign = 1;
            int ep = 0;

            for (; it; ++it)
            {
                Token child = *it;

                if (child.rule() == plusMinus)
                {
                    if (text.at(child) == '-')
                        sign = -1;
                }
                else if (child.rule() == integralPart)
                {
                    for (long j = child.range()[0]; j < child.range()[1]; ++j) {
                        mantissa *= 10;
                        mantissa += text.at(j) - '0';
                    }
                }
                else if (child.rule() == fractionalPart)
                {
                    double h = 0.1;
                    for (long j = child.range()[0]; j < child.range()[1]; ++j) {
                        mantissa += h * (text.at(j) - '0');
                        h /= 10;
                    }
                }
                else if (child.rule() == exponentPlusMinus)
                {
                    if (text.at(child) == '-')
                        epSign = -1;
                }
                else if (child.rule() == exponent)
                {
                    for (long j = child.range()[0]; j < child.range()[1]; ++j)
                    {
                        ep *= 10;
                        ep += text.at(j) - '0';
                    }
                }
            }

            value = sign * mantissa;
            if (ep != 0)
                value *= std::pow(10., double(epSign * ep));
        }
    }

    Token read(Out<double> value, const String &text, double offset) const
    {
        String section = offset == 0 ? text : String{text}.select(offset, text.count());
        Token token = match(section);
        if (token) read(value, text, token);
        return token;
    }
};

FloatSyntax::FloatSyntax()
{
    initOnce<State>();
}

void FloatSyntax::read(Out<double> value, const String &text, const Token &token) const
{
    me().read(value, text, token);
}

Token FloatSyntax::read(Out<double> value, const String &text, double offset) const
{
    return me().read(value, text, offset);
}

const FloatSyntax::State &FloatSyntax::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
