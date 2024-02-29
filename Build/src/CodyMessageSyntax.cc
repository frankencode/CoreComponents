/*
 * Copyright (C) 2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/CodyMessageSyntax>
#include <cc/SyntaxDefinition>
#include <cc/Format>

namespace cc::build {

struct CodyMessageSyntax::State final: public SyntaxDefinition::State
{
    SyntaxRule word {
        Repeat{1,
            Choice{
                Within{'a', 'z'},
                Within{'0', '9'},
                Within{'A', 'Z'},
                OneOf{"-+_/%."}
            }
        }
    };

    SyntaxRule quoutedWord {
        Sequence{
            Char{'\''},
            Repeat{
                Choice{
                    Other{'\''},
                    Literal{"\\'"}
                }
            },
            Char{'\''}
        }
    };

    SyntaxRule message {
        Sequence {
            Repeat{
                Choice{
                    &word,
                    &quoutedWord,
                    OneOf{" \t"}
                }
            },
            Repeat{0, 1, Char{'\n'}},
        }
    };

    State():
        SyntaxDefinition::State(&message)
    {}

    List<String> read(const String &line) const
    {
        Token token = match(line);
        if (!token) throw CodyError{Format{"Invalid message syntax: \"%%\""} << line};
        List<String> message;
        for (const Token &token: token.children()) {
            String word;
            if (token.rule() == quoutedWord) {
                word = line.copy(token.i0() + 1, token.i1() - 1);
                word.expand();
            }
            else {
                word = line.copy(token.i0(), token.i1());
            }
            message.append(word);
        }
        return message;
    }
};

CodyMessageSyntax::CodyMessageSyntax()
{
    initOnce<State>();
}

List<String> CodyMessageSyntax::read(const String &line) const
{
    return me().read(line);
}

const CodyMessageSyntax::State &CodyMessageSyntax::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::build
