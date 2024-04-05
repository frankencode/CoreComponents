/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/isobus/pd/TypeInfoSyntax>

namespace cc::isobus::pd {

struct TypeInfoSyntax::State final: public SyntaxDefinition::State
{
    SyntaxRule key_ {
        Sequence{
            Repeat{1,
                Choice{
                    Within{'a', 'z'},
                    Within{'A', 'Z'},
                    OneOf{' ', '-'}
                }
            },
            Ahead{':'}
        }
    };

    SyntaxRule value_ {
        Repeat{
            Sequence{
                Not{
                    Sequence{
                        '\n',
                        Inline{&key_}
                    }
                },
                Not{"\n\n\n"},
                Any{}
            }
        }
    };

    SyntaxRule keyValuePair_ {
        Sequence{
            &key_,
            ':',
            Repeat{' '},
            &value_
        }
    };

    SyntaxRule entry_ {
        Sequence{
            Repeat{1,
                Sequence{
                    &keyValuePair_,
                    '\n'
                }
            },
            Repeat{1, '\n'}
        }
    };

    State():
        SyntaxDefinition::State{&entry_}
    {}
};

TypeInfoSyntax::TypeInfoSyntax()
{
    initOnce<State>();
}

} // namespace cc::isobus::pd
