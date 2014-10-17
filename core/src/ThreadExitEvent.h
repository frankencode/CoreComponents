/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_THREADEXITEVENT_H
#define FLUX_THREADEXITEVENT_H

#include <flux/ThreadLocalSingleton>
#include <flux/Event>
#include <flux/Process>

namespace flux {

class ThreadExitEvent: public Event, public ThreadLocalSingleton<ThreadExitEvent>
{
private:
    friend class ThreadLocalSingleton<ThreadExitEvent>;

    ThreadExitEvent();
    ~ThreadExitEvent();

    pid_t pid_;
};

inline ThreadExitEvent *threadExitEvent() { return ThreadExitEvent::instance(); }

} // namespace flux

#endif // FLUX_THREADEXITEVENT_H
