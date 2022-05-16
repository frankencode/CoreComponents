/*
 * Copyright (C) 2007-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/toki/ShDiscoverySyntax>

namespace cc::toki {

struct ShDiscoverySyntax::State final: public SyntaxDefinition::State
{
    SyntaxRule firstLine_ {
        Sequence{
            "#!",
            Find{
                Choice{
                    '\n',
                    Ahead{"sh"}
                }
            },
            "sh"
        }
    };

    State():
        SyntaxDefinition::State{&firstLine_}
    {}
};

ShDiscoverySyntax::ShDiscoverySyntax()
{
    initOnce<State>();
}

} // namespace cc::toki
