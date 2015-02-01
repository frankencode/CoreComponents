/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_EXITEVENT_H
#define FLUX_EXITEVENT_H

#include <flux/Event>
#include <flux/Process>

namespace flux {

template<class SubClass> class Singleton;

class ExitEvent: public Event
{
protected:
    friend class Singleton<ExitEvent>;

    ExitEvent();
    ~ExitEvent();

    pid_t pid_;
};

ExitEvent *exitEvent();

} // namespace flux

#endif // FLUX_EXITEVENT_H
