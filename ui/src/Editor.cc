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

bool Editor::paste(const Range &range, const String &newChunk)
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
    Range range = delta->oldRange();
    text_ = text_()->paste(range->i0(), range->i1(), delta->oldChunk());
    return range;
}

Range Editor::redo()
{
    if (!canRedo()) return Range{};
    auto delta = future_->popFront();
    past_->pushBack(delta);
    Range range = delta->newRange();
    text_ = text_()->paste(range->i0(), range->i1(), delta->newChunk());
    return range;
}

}} // namespace cc::ui
