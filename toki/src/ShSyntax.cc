/*
 * Copyright (C) 2007-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/toki/ShSyntax>

namespace cc::toki {

struct ShSyntax::State final: public HighlightingSyntax::State
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
        HighlightingSyntax::State{&source_}
    {}
};

ShSyntax::ShSyntax()
{
    initOnce<State>();
}

} // namespace cc::toki
