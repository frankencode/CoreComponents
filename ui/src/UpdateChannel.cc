/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Mutex>
#include <cc/Guard>
#include <cc/WaitCondition>
#include <cc/ui/UpdateChannel>

namespace cc {
namespace ui {

UpdateChannel::UpdateChannel():
    queue_(Queue::create()),
    mutex_(Mutex::create()),
    counter_(0)
{}

UpdateChannel::~UpdateChannel()
{}

void UpdateChannel::push(Window *window)
{
    Guard<Mutex> guard(mutex_);
    queue_->insert(window);
}

Ref<Window> UpdateChannel::pop()
{
    Guard<Mutex> guard(mutex_);
    if (queue_->count() == 0) return 0;
    int index = (counter_ ++) % queue_->count();
    Ref<Window> window = queue_->at(index);
    queue_->removeAt(index);
    return window;
}

}} // namespace cc::ui
