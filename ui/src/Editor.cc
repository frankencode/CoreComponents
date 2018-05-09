/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Editor>

namespace cc {
namespace ui {

Editor::Editor(const String &initialText):
    text_{initialText},
    past_{History::create()},
    future_{History::create()}
{}

bool Editor::paste(Range range, const String &newChunk)
{
    if (!(0 <= range->i0() && range->i1() <= text_()->count())) return false;

    future_->deplete();
    past_->pushBack(
        Object::create<Delta>(
            range->i0(),
            text_()->copy(range->i0(), range->i1()),
            newChunk
        )
    );
    text_ = text_()->paste(range->i0(), range->i1(), newChunk);

    return true;
}

bool Editor::canUndo() const
{
    return past_->count() > 0;
}

bool Editor::canRedo() const
{
    return future_->count() > 0;
}

Range Editor::undo()
{
    if (!canUndo()) return Range{};
    auto delta = past_->popBack();
    future_->pushFront(delta);
    Range newRange = delta->newRange();
    text_ = text_()->paste(newRange->i0(), newRange->i1(), delta->oldChunk());
    return delta->oldRange();
}

Range Editor::redo()
{
    if (!canRedo()) return Range{};
    auto delta = future_->popFront();
    past_->pushBack(delta);
    Range oldRange = delta->oldRange();
    text_ = text_()->paste(oldRange->i0(), oldRange->i1(), delta->newChunk());
    return delta->newRange();
}

}} // namespace cc::ui
