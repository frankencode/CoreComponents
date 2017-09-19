/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/ui/View>
#include <cc/ui/PaintCommand>

namespace cc {
namespace ui {

class Frame: public Object
{
public:
    inline static Ref<Frame> create(View *view) {
        return new Frame(view);
    }

    inline View *view() const { return view_; }

    inline void pushPaintCommand(PaintCommand *cmd) { queue_->push(cmd); }
    inline bool popPaintCommand(Ref<PaintCommand> *cmd) {
        if (queue_->count() == 0) return false;
        queue_->pop(cmd);
        return true;
    }

private:
    Frame(View *view):
        view_(view),
        queue_(PaintQueue::create())
    {}

    Ref<View> view_;
    Ref<PaintQueue> queue_;
};

}} // namespace cc::ui

