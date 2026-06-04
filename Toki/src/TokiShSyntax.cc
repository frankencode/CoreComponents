/*
 * Copyright (C) 2007-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/TokiShSyntax>

namespace cc {

struct TokiShSyntax::State final: public TokiHighlightingSyntax::State
{
    SyntaxRule comment_ {
        Sequence{
            '#',
            Find{
                Choice{
                    '\n',
                    Eoi{}
                }
            }
        }
    };

    SyntaxRule source_ {
        Repeat{
            Choice{
                &comment_,
                Any{}
            }
        }
    };

    State():
        TokiHighlightingSyntax::State{&source_}
    {}
};

TokiShSyntax::TokiShSyntax()
{
    initOnce<State>();
}

} // namespace cc
