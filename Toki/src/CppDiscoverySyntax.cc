/*
 * Copyright (C) 2007-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/toki/CppDiscoverySyntax>
#include <cc/toki/CxxSyntax>

namespace cc::toki {

struct CppDiscoverySyntax::State final: public SyntaxDefinition::State
{
    SyntaxRule header_ {
        Sequence{
            Repeat{
                Choice{
                    Repeat{1, OneOf{" \n\t\r"}},
                    &CxxSyntax{}.commentText(),
                    &CxxSyntax{}.commentLine()
                }
            },
            '#',
            Repeat{OneOf{" \t"}},
            Keyword{"ifndef", "include", "pragma"}
        }
    };

    State():
        SyntaxDefinition::State{&header_}
    {}
};

CppDiscoverySyntax::CppDiscoverySyntax()
{
    initOnce<State>();
}

} // namespace cc::toki
