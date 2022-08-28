/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/CsvSyntax>

namespace cc {

struct CsvSyntax::State: public SyntaxDefinition::State
{
    SyntaxRule nonEscaped {
        Repeat{
            NoneOf{",\"\n\f"}
        }
    };

    SyntaxRule escaped {
        Sequence{
            '\"',
            Repeat{
                Choice{
                    Other{'\"'},
                    Literal{"\"\""}
                }
            },
            '\"'
        }
    };

    SyntaxRule field {
        Choice{
            &escaped,
            Inline{&nonEscaped}
        }
    };

    SyntaxRule record {
        Sequence{
            &field,
            Repeat{
                Sequence{
                    ',',
                    &field
                }
            },
            Choice{
                Repeat{1, OneOf{"\n\f"}},
                Eoi{}
            }
        }
    };

    State():
        SyntaxDefinition::State{&record}
    {}

    List<String> parse(const String &text, InOut<long> offset) const
    {
        if (offset() < 0 || text.count() <= offset()) return List<String>{};

        Token recordToken = SyntaxDefinition::State::parse(text, offset());
        offset = recordToken.i1();

        List<String> items;

        for (const Token &fieldToken: recordToken.children()) {
            String item;
            if (fieldToken.children()) {
                item = text.copy(fieldToken.i0() + 1, fieldToken.i1() - 1);
                item.replace("\"\"", "\"");
            }
            else {
                item = text.copy(fieldToken.i0(), fieldToken.i1());
            }
            items.append(item);
        }

        return items;
    }
};

CsvSyntax::CsvSyntax()
{
    initOnce<State>();
}

List<String> CsvSyntax::parse(const String &text, InOut<long> offset) const
{
    return me().parse(text, offset);
}

const CsvSyntax::State &CsvSyntax::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
