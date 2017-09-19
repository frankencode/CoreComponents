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
#include <cc/ui/RenderQueue>

namespace cc {
namespace ui {

RenderQueue::RenderQueue():
    queue_(Queue::create()),
    mutex_(Mutex::create()),
    notEmpty_(WaitCondition::create()),
    counter_(0)
{}

RenderQueue::~RenderQueue()
{}

void RenderQueue::pushFrame(Frame *frame)
{
    Guard<Mutex> guard(mutex_);
    queue_->establish(frame->view(), frame);
    notEmpty_->signal();
}

bool RenderQueue::popFrame(Ref<Frame> *frame)
{
    Guard<Mutex> guard(mutex_);
    while (queue_->count() == 0)
        notEmpty_->wait(mutex_);
    int index = (counter_ ++) % queue_->count();
    *frame = queue_->valueAt(index);
    queue_->removeAt(index);
    return *frame;
}

void RenderQueue::shutdown()
{
    queue_->insert(0, 0);
}

}} // namespace cc::ui
