/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/TextEditorWithHistory>

namespace cc {
namespace ui {

TextEditorWithHistory::TextEditorWithHistory():
    past_{History::create()},
    future_{History::create()}
{}

String TextEditorWithHistory::text() const
{
    return copy(Range{0, byteCount()});
}

Range TextEditorWithHistory::paste(Range range, const String &newChunk)
{
    if (!(0 <= range->i0() && range->i1() <= byteCount())) return Range{};

    String filteredChunk = filterChunk(newChunk);

    future_->deplete();
    auto delta =
        Object::create<TextDelta>(
            range->i0(),
            copy(range),
            filteredChunk
        );
    past_->pushBack(delta);

    pasteChunk(range, filteredChunk);

    return delta->newRange();
}

bool TextEditorWithHistory::canUndo() const
{
    return past_->count() > 0;
}

bool TextEditorWithHistory::canRedo() const
{
    return future_->count() > 0;
}

Range TextEditorWithHistory::undo()
{
    if (!canUndo()) return Range{};
    auto delta = past_->popBack();
    future_->pushFront(delta);
    Range newRange = delta->newRange();
    pasteChunk(newRange, delta->oldChunk());
    return delta->oldRange();
}

Range TextEditorWithHistory::redo()
{
    if (!canRedo()) return Range{};
    auto delta = future_->popFront();
    past_->pushBack(delta);
    Range oldRange = delta->oldRange();
    pasteChunk(oldRange, delta->newChunk());
    return delta->newRange();
}

String TextEditorWithHistory::filterChunk(const String &newChunk) const
{
    return newChunk;
}

}} // namespace cc::ui
