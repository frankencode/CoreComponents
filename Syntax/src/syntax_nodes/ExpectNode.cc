/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/syntax_node/ExpectNode>
#include <cc/syntax_node/CharNode>
#include <cc/syntax_node/LiteralNode>
#include <cc/Format>

namespace cc::syntax {

struct ExpectNode::State: public SyntaxNode::State
{
    State(const SyntaxNode &entry, const String &error):
        entry_{entry},
        error_{error}
    {}

    long matchNext(const String &text, long offset, Token &production) const override
    {
        long i = entry_.matchNext(text, offset, production);

        if (i == -1) {
            String message = error_;
            if (message == "") {
                String hint = entry_.hint();
                if (hint != "") message = Format{"Expected \"%%\""} << hint;
            }
            throw TextError{text, offset, message};
        }

        return i;
    }

    long matchLength() const override
    {
        return entry_.matchLength();
    }

    void decycle() override
    {
        entry_ = SyntaxNode{};
    }

    List<String> explain() const override
    {
        return List<String>{} <<
            "Expect{" << entry_.explain() << ",\"" << error_ << "\"" << "}";
    }

    SyntaxNode entry_;
    String error_;
};

ExpectNode::ExpectNode(const SyntaxNode &entry, const String &error):
    SyntaxNode{new State{entry, error}}
{}

ExpectNode::ExpectNode(char ch, const String &error):
    ExpectNode{CharNode{ch}, error}
{}

ExpectNode::ExpectNode(const String &s, const String &error):
    ExpectNode{LiteralNode{s}, error}
{}

} // namespace cc::syntax
