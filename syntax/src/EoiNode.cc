/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/syntax/EoiNode>

namespace cc::syntax {

struct EoiNode::State: public SyntaxNode::State
{
    long matchNext(const String &text, long offset, Token &production) const override
    {
        bool eoi = (!text.has(offset)) && ((offset == 0) || (text.has(offset - 1)));
        return eoi ? offset : -1;
    }

    long matchLength() const override
    {
        return 0;
    }

    List<String> explain() const override
    {
        return List<String>{} << "Eoi{}";
    }
};

EoiNode::EoiNode():
    SyntaxNode{new State}
{}

} // namespace cc::syntax
