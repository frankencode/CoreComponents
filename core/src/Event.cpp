/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/Guard>
#include <flux/Event>

namespace flux {

Event::Event()
    : handlers_(Handlers::create())
{}

void Event::pushBack(Action *handler)
{
    Guard<SpinLock> guard(&mutex_);
    handlers_->pushBack(handler);
}

void Event::pushFront(Action *handler)
{
    Guard<SpinLock> guard(&mutex_);
    handlers_->pushFront(handler);
}

void Event::remove(Action *handler)
{
    Guard<SpinLock> guard(&mutex_);
    int i = handlers_->find(handler);
    if (i < handlers_->count())
        handlers_->remove(i);
}

void Event::run()
{
    Guard<SpinLock> guard(&mutex_);
    for (int i = 0; i < handlers_->count(); ++i)
        handlers_->at(i)->run();
}

} // namespace flux
