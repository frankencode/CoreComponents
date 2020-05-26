/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/TextDelta>
#include <cc/ui/LineEditor>

namespace cc {
namespace ui {

LineEditor::LineEditor(const string &initialText):
    text_{initialText}
{}

string LineEditor::text() const
{
    return text_();
}

int LineEditor::byteCount() const
{
    return text_()->count();
}

string LineEditor::copy(Range range) const
{
    return text_()->copy(range->i0(), range->i1());
}

void LineEditor::pasteChunk(Range range, const string &newChunk)
{
    text_ = text_()->paste(range->i0(), range->i1(), newChunk);
}

}} // namespace cc::ui
