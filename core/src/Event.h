/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_EVENT_H
#define FLUX_EVENT_H

#include <flux/List>
#include <flux/SpinLock>

namespace flux {

class Event: public Action
{
public:
    inline static Ref<Event> create() {
        return new Event;
    }

    void pushBack(Action *handler);
    void pushFront(Action *handler);
    void remove(Action *handler);

    virtual void run();

protected:
    Event();

private:
    SpinLock mutex_;
    typedef List< Ref<Action> > Handlers;
    Ref<Handlers> handlers_;
};

} // namespace flux

#endif // FLUX_EVENT_H
