/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/IntegerSyntax>
#include <cc/InOut>

namespace cc {

struct IntegerSyntax::State final: public SyntaxDefinition::State
{
    SyntaxRule plusMinus {
        OneOf{'+', '-'}
    };

    SyntaxRule binary {
        Sequence {
            Literal{"0b"},
            Repeat{1, 256, OneOf{'0', '1'}}
        }
    };

    SyntaxRule octal {
        Sequence {
            Char{'0'},
            Repeat{1, 24, Within{'0', '9'}}
        }
    };

    SyntaxRule hexadecimal {
        Sequence {
            Literal{"0x"},
            Repeat{1, 20,
                Choice{
                    Within{'0', '9'},
                    Within{'a', 'f'},
                    Within{'A', 'F'}
                }
            }
        }
    };

    SyntaxRule decimal {
        Repeat{1, 20,
            Within{'0', '9'}
        }
    };

    SyntaxRule integer {
        Sequence{
            Repeat{0, 1, &plusMinus},
            Choice{
                &binary,
                &octal,
                &hexadecimal,
                &decimal
            },
            Not{
                OneOf{'.', 'e', 'E'}
            }
        }
    };

    State():
        SyntaxDefinition::State{&integer}
    {}

    void read(Out<uint64_t> value, Out<int> sign, const String &text, const Token &token) const
    {
        sign = 1;
        value = 0;

        Token child = token.children().first();

        if (child.rule() == plusMinus)
        {
            if (text.at(child) == '-') sign = -1;
            child = *(++token.children().begin());
        }

        if (child.rule() == decimal)
        {
            for (long i = child.range()[0]; i < child.range()[1]; ++i)
            {
                value *= 10;
                value += text.at(i) - '0';
            }
        }
        else if (child.rule() == hexadecimal)
        {
            for (long i = child.range()[0] + 2; i < child.range()[1]; ++i)
            {
                char x = text.at(i);
                if (('0' <= x) && (x <= '9')) x -= '0';
                else if (('a' <= x) && (x <= 'z')) x -= 'a' - 10;
                else if (('A' <= x) && (x <= 'Z')) x -= 'A' - 10;
                value *= 16;
                value += x;
            }
        }
        else if (child.rule() == binary)
        {
            for (long i = child.range()[0] + 2; i < child.range()[1]; ++i)
            {
                value *= 2;
                value += text.at(i) - '0';
            }
        }
        else if (child.rule() == octal)
        {
            for (long i = child.range()[0] + 1; i < child.range()[1]; ++i)
            {
                value *= 8;
                value += text.at(i) - '0';
            }
        }
    }

    Token read(Out<uint64_t> value, Out<int> sign, const String &text, long offset) const
    {
        String section = (offset == 0) ? text : String{text}.select(offset, text.count());
        Token token = match(section);
        if (token) read(value, sign, text, token);
        return token;
    }
};

IntegerSyntax::IntegerSyntax()
{
    initOnce<State>();
}

void IntegerSyntax::read(Out<uint64_t> value, Out<int> sign, const String &text, const Token &token) const
{
    me().read(value, sign, text, token);
}

Token IntegerSyntax::read(Out<uint64_t> value, Out<int> sign, const String &text, long offset) const
{
    return me().read(value, sign, text, offset);
}

const IntegerSyntax::State &IntegerSyntax::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
