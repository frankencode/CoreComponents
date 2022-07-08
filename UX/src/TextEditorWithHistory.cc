/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/TextEditorWithHistory>

namespace cc {

String TextEditorWithHistory::State::filterChunk(const String &chunk) const
{
    return chunk;
}

Range TextEditorWithHistory::State::paste(Range range, const String &chunk)
{
    if (!(0 <= range.i0() && range.i1() <= count())) return Range{};

    String filteredChunk = filterChunk(chunk);

    future_.deplete();
    TextDelta delta{range.i0(), copy(range), filteredChunk};
    past_.pushBack(delta);

    pasteChunk(range, filteredChunk);

    return delta.newRange();
}

bool TextEditorWithHistory::State::canUndo() const
{
    return past_.count() > 0;
}

bool TextEditorWithHistory::State::canRedo() const
{
    return future_.count() > 0;
}

Range TextEditorWithHistory::State::undo()
{
    if (!canUndo()) return Range{};
    TextDelta delta = past_.last();
    past_.popBack();
    future_.pushFront(delta);
    Range newRange = delta.newRange();
    pasteChunk(newRange, delta.oldChunk());
    return delta.oldRange();
}

Range TextEditorWithHistory::State::redo()
{
    if (!canRedo()) return Range{};
    TextDelta delta = future_.first();
    future_.popFront();
    past_.pushBack(delta);
    Range oldRange = delta.oldRange();
    pasteChunk(oldRange, delta.newChunk());
    return delta.newRange();
}

} // namespace cc
