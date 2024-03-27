/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/syntax_node/EoiNode>

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
