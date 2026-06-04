/*
 * Copyright (C) 2007-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/TokiShDiscoverySyntax>

namespace cc {

struct TokiShDiscoverySyntax::State final: public SyntaxDefinition::State
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

TokiShDiscoverySyntax::TokiShDiscoverySyntax()
{
    initOnce<State>();
}

} // namespace cc
