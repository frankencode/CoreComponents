/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/syntax/DebugNode>
#include <cc/stdio>

namespace cc::syntax {

struct DebugNode::State: public SyntaxNode::State
{
    State(const SyntaxNode &entry, const String &label):
        entry_{entry},
        label_{label}
    {
        if (label_ == "") label_ = "DEBUG";
    }

    long matchNext(const String &text, long offset, Token &production) const override
    {
        long newOffset = entry_.matchNext(text, offset, production);
        String chunk;
        if (0 <= offset && offset < newOffset && newOffset <= text.count()) chunk = text.copy(offset, newOffset);
        ferr() << label_ << ":" << offset << ".." << newOffset << ": \"" << chunk << "\""<< nl;
        return newOffset;
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
            "Debug{" << entry_.explain() << ",\"" << label_ << "\"" << "}";
    }

    SyntaxNode entry_;
    String label_;
};

DebugNode::DebugNode(const SyntaxNode &entry, const String &label):
    SyntaxNode{new State{entry, label}}
{}

} // namespace cc::syntax
