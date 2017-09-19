/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Set>
#include <cc/ui/Window>

namespace cc {
    class Mutex;
    class WaitCondition;
}

namespace cc {
namespace ui {

class UpdateChannel: public Object
{
public:
    inline static Ref<UpdateChannel> create() {
        return new UpdateChannel;
    }

    void push(Window *window);
    Ref<Window> pop();

private:
    UpdateChannel();
    ~UpdateChannel();

    typedef Set< Ref<Window> > Queue;
    Ref<Queue> queue_;

    Ref<Mutex> mutex_;
    unsigned counter_;
};

}} // namespace cc::ui

