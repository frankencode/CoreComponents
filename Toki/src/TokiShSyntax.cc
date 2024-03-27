/*
 * Copyright (C) 2007-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
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
