/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/TextLineEditor>
#include <cc/Property>

namespace cc {

struct TextLineEditor::State: public TextEditorWithHistory::State
{
    State(const String &text):
        text_{text}
    {}

private:
    long count() const override
    {
        return text_().count();
    }

    String copy(Range range) const override
    {
        return text_().copy(range.i0(), range.i1());
    }

    String text() const override
    {
        return text_();
    }

    void pasteChunk(Range range, const String &chunk) override
    {
        text_ = text_().paste(range.i0(), range.i1(), chunk);
    }

    Property<String> text_;
};

TextLineEditor::TextLineEditor(const String &text):
    TextEditorWithHistory{new State{text}}
{}

} // namespace cc
