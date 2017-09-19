/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Map>
#include <cc/ui/View>
#include <cc/ui/Frame>
#include <cc/ui/PaintCommand>

namespace cc {
    class Mutex;
    class WaitCondition;
}

namespace cc {
namespace ui {

class RenderQueue: public Object
{
public:
    inline static Ref<RenderQueue> create() {
        return new RenderQueue();
    }

    void pushFrame(Frame *frame);
    bool popFrame(Ref<Frame> *frame);

    void shutdown();

private:
    RenderQueue();
    ~RenderQueue();

    typedef Map< View *, Ref<Frame> > Queue;
    Ref<Queue> queue_;

    Ref<Mutex> mutex_;
    Ref<WaitCondition> notEmpty_;

    unsigned counter_;
};

}} // namespace cc::ui

