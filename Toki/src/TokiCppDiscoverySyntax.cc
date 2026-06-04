/*
 * Copyright (C) 2007-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/TokiCppDiscoverySyntax>
#include <cc/TokiCxxSyntax>

namespace cc {

struct TokiCppDiscoverySyntax::State final: public SyntaxDefinition::State
{
    SyntaxRule header_ {
        Sequence{
            Repeat{
                Choice{
                    Repeat{1, OneOf{" \n\t\r"}},
                    &TokiCxxSyntax{}.commentText(),
                    &TokiCxxSyntax{}.commentLine()
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

TokiCppDiscoverySyntax::TokiCppDiscoverySyntax()
{
    initOnce<State>();
}

} // namespace cc
